/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2014 Fred Hoogduin
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "pvrclient-argustv.h"

#include "activerecording.h"
#include "addon.h"
#include "argustvrpc.h"
#include "channel.h"
#include "epg.h"
#include "lib/tsreader/TSReader.h"
#include "recording.h"
#include "recordinggroup.h"
#include "upcomingrecording.h"
#include "utils.h"

#include <chrono>
#include <kodi/General.h>
#include <kodi/tools/StringUtils.h>
#include <map>
#include <thread>

using namespace ArgusTV;

#define SIGNALQUALITY_INTERVAL 10
#define MAXLIFETIME \
  99 //Based on VDR addon and VDR documentation. 99=Keep forever, 0=can be deleted at any time, 1..98=days to keep

template<typename T>
void SafeDelete(T*& p)
{
  if (p)
  {
    delete p;
    p = nullptr;
  }
}

/************************************************************/
/** Class interface */

cPVRClientArgusTV::cPVRClientArgusTV(const CArgusTVAddon& base,
                                     const kodi::addon::IInstanceInfo& instance)
  : kodi::addon::CInstancePVRClient(instance), m_base(base)
{
#if defined(ATV_DUMPTS)
  strncpy(ofn, "/tmp/atv.XXXXXX", sizeof(ofn));
  ofd = -1;
#endif
}

cPVRClientArgusTV::~cPVRClientArgusTV()
{
  kodi::Log(ADDON_LOG_DEBUG, "->~cPVRClientArgusTV()");
  // Check if we are still reading a TV/Radio stream and close it here
  if (m_bTimeShiftStarted)
  {
    CloseLiveStream();
  }
  delete m_keepalive;
  delete m_eventmonitor;
  // Free allocated memory for Channels
  FreeChannels(m_TVChannels);
  FreeChannels(m_RadioChannels);
}

PVR_ERROR cPVRClientArgusTV::GetCapabilities(kodi::addon::PVRCapabilities& capabilities)
{
  kodi::Log(ADDON_LOG_DEBUG, "->GetCapabilities()");

  capabilities.SetSupportsEPG(true);
  capabilities.SetSupportsRecordings(true);
  capabilities.SetSupportsRecordingsDelete(true);
  capabilities.SetSupportsRecordingsUndelete(false);
  capabilities.SetSupportsTimers(true);
  capabilities.SetSupportsTV(true);
  capabilities.SetSupportsRadio(m_base.GetSettings().RadioEnabled());
  capabilities.SetSupportsChannelGroups(true);
  capabilities.SetHandlesInputStream(true);
  capabilities.SetHandlesDemuxing(false);
  capabilities.SetSupportsChannelScan(false);
  capabilities.SetSupportsLastPlayedPosition(true);
  capabilities.SetSupportsRecordingPlayCount(true);
  capabilities.SetSupportsRecordingsRename(true);
  capabilities.SetSupportsRecordingsLifetimeChange(false);
  capabilities.SetSupportsDescrambleInfo(false);
  capabilities.SetSupportsRecordingEdl(true);

  return PVR_ERROR_NO_ERROR;
}

bool cPVRClientArgusTV::Connect()
{
  m_baseURL = m_base.GetSettings().BaseURL();

  kodi::Log(ADDON_LOG_INFO, "Connect() - Connecting to %s", m_baseURL.c_str());

  m_rpc.Initialize(m_baseURL);

  int backendversion = ATV_REST_MAXIMUM_API_VERSION;
  int rc = -2;
  int attemps = 0;

  while (rc != 0)
  {
    attemps++;
    rc = m_rpc.Ping(backendversion);
    if (rc == 1)
    {
      backendversion = ATV_REST_MINIMUM_API_VERSION;
      rc = m_rpc.Ping(backendversion);
    }
    m_iBackendVersion = backendversion;

    switch (rc)
    {
      case 0:
        kodi::Log(ADDON_LOG_INFO, "Ping Ok. The client and server are compatible, API version %d.",
                  m_iBackendVersion);
        break;
      case -1:
        kodi::Log(ADDON_LOG_INFO,
                  "Ping Ok. The ARGUS TV server is too new for this version of the add-on.");
        kodi::QueueNotification(QUEUE_ERROR, "",
                                "The ARGUS TV server is too new for this version of the add-on");
        return false;
      case 1:
        kodi::Log(ADDON_LOG_INFO,
                  "Ping Ok. The ARGUS TV server is too old for this version of the add-on.");
        kodi::QueueNotification(QUEUE_ERROR, "",
                                "The ARGUS TV server is too old for this version of the add-on");
        return false;
      default:
        kodi::Log(ADDON_LOG_ERROR, "Ping failed... No connection to Argus TV.");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (attemps > 3)
        {
          return false;
        }
    }
  }

  // Check the accessibility status of all the shares used by ArgusTV tuners
  // TODO: this is temporarily disabled until the caching of smb:// directories is resolved
  //  if (ShareErrorsFound())
  //  {
  //    kodi::QueueNotification(QUEUE_ERROR, "", "Share errors: see xbmc.log");
  //  }

  // Start service events monitor
  m_eventmonitor->Connect();
  m_eventmonitor->StartThread();
  m_bConnected = true;
  return true;
}

void cPVRClientArgusTV::Disconnect()
{
  std::string result;

  kodi::Log(ADDON_LOG_INFO, "Disconnect");

  // Stop service events monitor
  m_eventmonitor->StopThread();

  if (m_bTimeShiftStarted)
  {
    //TODO: tell ArgusTV that it should stop streaming
  }

  m_bConnected = false;
}

bool cPVRClientArgusTV::ShareErrorsFound(void)
{
  bool bShareErrors = false;
  Json::Value activeplugins;
  int rc = m_rpc.GetPluginServices(false, activeplugins);
  if (rc < 0)
  {
    kodi::Log(ADDON_LOG_ERROR,
              "Unable to get the ARGUS TV plugin services to check share accessiblity.");
    return false;
  }

  // parse plugins list
  int size = activeplugins.size();
  for (int index = 0; index < size; ++index)
  {
    std::string tunerName = activeplugins[index]["Name"].asString();
    kodi::Log(ADDON_LOG_DEBUG, "Checking tuner \"%s\" for accessibility.", tunerName.c_str());
    Json::Value accesibleshares;
    rc = m_rpc.AreRecordingSharesAccessible(activeplugins[index], accesibleshares);
    if (rc < 0)
    {
      kodi::Log(ADDON_LOG_ERROR, "Unable to get the share status for tuner \"%s\".",
                tunerName.c_str());
      continue;
    }
    int numberofshares = accesibleshares.size();
    for (int j = 0; j < numberofshares; j++)
    {
      Json::Value accesibleshare = accesibleshares[j];
      tunerName = accesibleshare["RecorderTunerName"].asString();
      std::string sharename = accesibleshare["Share"].asString();
      bool isAccessibleByATV = accesibleshare["ShareAccessible"].asBool();
      bool isAccessibleByAddon = false;
      std::string accessMsg = "";
      std::string CIFSname = ToCIFS(sharename);
      std::vector<kodi::vfs::CDirEntry> items;
      isAccessibleByAddon = kodi::vfs::GetDirectory(CIFSname, "", items);
      // write analysis results to the log
      if (isAccessibleByATV)
      {
        kodi::Log(ADDON_LOG_DEBUG, "  Share \"%s\" is accessible to the ARGUS TV server.",
                  sharename.c_str());
      }
      else
      {
        bShareErrors = true;
        kodi::Log(ADDON_LOG_ERROR, "  Share \"%s\" is NOT accessible to the ARGUS TV server.",
                  sharename.c_str());
      }
      if (isAccessibleByAddon)
      {
        kodi::Log(ADDON_LOG_DEBUG, "  Share \"%s\" is readable from this client add-on.",
                  sharename.c_str());
      }
      else
      {
        bShareErrors = true;
        kodi::Log(ADDON_LOG_ERROR,
                  "  Share \"%s\" is NOT readable from this client add-on (\"%s\").",
                  sharename.c_str(), accessMsg.c_str());
      }
    }
  }
  return bShareErrors;
}

/************************************************************/
/** General handling */

// Used among others for the server name string in the "Recordings" view
PVR_ERROR cPVRClientArgusTV::GetBackendName(std::string& name)
{
  kodi::Log(ADDON_LOG_DEBUG, "->GetBackendName()");

  name = "ARGUS TV (" + m_base.GetSettings().Hostname() + ")";
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetBackendVersion(std::string& version)
{
  kodi::Log(ADDON_LOG_DEBUG, "->GetBackendVersion");
  Json::Value response;

  int retval = m_rpc.GetDisplayVersion(response);
  if (retval == E_FAILED)
    return PVR_ERROR_FAILED;

  version = response.asString();
  kodi::Log(ADDON_LOG_DEBUG, "GetDisplayVersion: \"%s\".", version.c_str());

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetConnectionString(std::string& connection)
{
  kodi::Log(ADDON_LOG_DEBUG, "->GetConnectionString()");

  connection = m_baseURL;

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetDriveSpace(uint64_t& total, uint64_t& used)
{
  kodi::Log(ADDON_LOG_DEBUG, "->GetDriveSpace");
  total = used = 0;
  Json::Value response;

  int retval = m_rpc.GetRecordingDisksInfo(response);
  if (retval != E_FAILED)
  {
    double _totalSize = response["TotalSizeBytes"].asDouble() / 1024;
    double _freeSize = response["FreeSpaceBytes"].asDouble() / 1024;
    total = (int64_t)_totalSize;
    used = (int64_t)(_totalSize - _freeSize);
    kodi::Log(ADDON_LOG_DEBUG, "GetDriveSpace, %lld used kiloBytes of %lld total kiloBytes.", used,
              total);
  }

  return PVR_ERROR_NO_ERROR;
}

/************************************************************/
/** EPG handling */

PVR_ERROR cPVRClientArgusTV::GetEPGForChannel(int channelUid,
                                              time_t start,
                                              time_t end,
                                              kodi::addon::PVREPGTagsResultSet& results)
{
  kodi::Log(ADDON_LOG_DEBUG, "->GetEPGForChannel(%i)", channelUid);

  cChannel* atvchannel = FetchChannel(channelUid);
  kodi::Log(ADDON_LOG_DEBUG, "ARGUS TV channel %p)", atvchannel);

  struct tm* convert = localtime(&start);
  struct tm tm_start = *convert;
  convert = localtime(&end);
  struct tm tm_end = *convert;

  if (atvchannel)
  {
    Json::Value response;
    int retval;

    kodi::Log(ADDON_LOG_DEBUG, "Getting EPG Data for ARGUS TV channel %s)",
              atvchannel->GuideChannelID().c_str());
    retval = m_rpc.GetEPGData(atvchannel->GuideChannelID(), tm_start, tm_end, response);

    if (retval != E_FAILED)
    {
      kodi::Log(ADDON_LOG_DEBUG,
                "GetEPGData returned %i, response.type == %i, response.size == %i.", retval,
                response.type(), response.size());
      if (response.type() == Json::arrayValue)
      {
        int size = response.size();
        kodi::addon::PVREPGTag broadcast;
        cEpg internalEpg;

        // parse channel list
        for (int index = 0; index < size; ++index)
        {
          if (internalEpg.Parse(response[index]))
          {
            m_epg_id_offset++;
            broadcast.SetUniqueBroadcastId(m_epg_id_offset);
            broadcast.SetTitle(internalEpg.Title());
            broadcast.SetUniqueChannelId(channelUid);
            broadcast.SetStartTime(internalEpg.StartTime());
            broadcast.SetEndTime(internalEpg.EndTime());
            broadcast.SetPlotOutline(internalEpg.Subtitle());
            broadcast.SetPlot(internalEpg.Description());
            broadcast.SetIconPath("");
            broadcast.SetGenreType(EPG_GENRE_USE_STRING);
            broadcast.SetGenreSubType(0);
            broadcast.SetGenreDescription(internalEpg.Genre());
            broadcast.SetFirstAired("");
            broadcast.SetParentalRating(0);
            broadcast.SetStarRating(0);
            broadcast.SetSeriesNumber(EPG_TAG_INVALID_SERIES_EPISODE);
            broadcast.SetEpisodeNumber(EPG_TAG_INVALID_SERIES_EPISODE);
            broadcast.SetEpisodePartNumber(EPG_TAG_INVALID_SERIES_EPISODE);
            broadcast.SetEpisodeName("");
            broadcast.SetOriginalTitle("");
            broadcast.SetCast("");
            broadcast.SetDirector("");
            broadcast.SetWriter("");
            broadcast.SetYear(0);
            broadcast.SetIMDBNumber("");
            broadcast.SetFlags(EPG_TAG_FLAG_UNDEFINED);

            results.Add(broadcast);
          }
          internalEpg.Reset();
        }
      }
    }
    else
    {
      kodi::Log(ADDON_LOG_ERROR, "GetEPGData failed for channel id:%i", channelUid);
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_ERROR, "Channel (%i) did not return a channel class.", channelUid);
    kodi::QueueNotification(QUEUE_ERROR, "", "Can't map XBMC Channel to ARGUS");
  }

  return PVR_ERROR_NO_ERROR;
}

/************************************************************/
/** Channel handling */

PVR_ERROR cPVRClientArgusTV::GetChannelsAmount(int& amount)
{
  // Not directly possible in ARGUS TV
  Json::Value response;

  kodi::Log(ADDON_LOG_DEBUG, "GetChannelsAmount()");

  // pick up the channellist for TV
  int retval = m_rpc.GetChannelList(CArgusTV::Television, response);
  if (retval < 0)
  {
    return PVR_ERROR_FAILED;
  }

  amount = response.size();

  // When radio is enabled, add the number of radio channels
  if (m_base.GetSettings().RadioEnabled())
  {
    retval = m_rpc.GetChannelList(CArgusTV::Radio, response);
    if (retval >= 0)
    {
      amount += response.size();
    }
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results)
{
  std::lock_guard<std::mutex> lock(m_ChannelCacheMutex);
  Json::Value response;
  int retval = -1;

  if (radio && !m_base.GetSettings().RadioEnabled())
    return PVR_ERROR_NO_ERROR;

  kodi::Log(ADDON_LOG_DEBUG, "%s(%s)", __FUNCTION__, radio ? "radio" : "television");
  if (!radio)
  {
    retval = m_rpc.GetChannelList(CArgusTV::Television, response);
  }
  else
  {
    retval = m_rpc.GetChannelList(CArgusTV::Radio, response);
  }

  if (retval >= 0)
  {
    if (radio)
    {
      FreeChannels(m_RadioChannels);
      m_RadioChannels.clear();
    }
    else
    {
      FreeChannels(m_TVChannels);
      m_TVChannels.clear();
    }
    int size = response.size();

    // parse channel list
    for (int index = 0; index < size; ++index)
    {

      cChannel* channel = new cChannel;
      if (channel->Parse(response[index]))
      {
        kodi::addon::PVRChannel tag;
        tag.SetUniqueId(channel->ID());
        tag.SetChannelName(channel->Name());
        tag.SetIconPath(m_rpc.GetChannelLogo(channel->Guid()));
        tag.SetEncryptionSystem((unsigned int)-1); //How to fetch this from ARGUS TV??
        tag.SetIsRadio(channel->Type() == CArgusTV::Radio ? true : false);
        tag.SetIsHidden(false);
        tag.SetMimeType("video/mp2t");
        tag.SetChannelNumber(channel->LCN());

        if (!tag.GetIsRadio())
        {
          m_TVChannels.push_back(channel);
          kodi::Log(
              ADDON_LOG_DEBUG,
              "Found TV channel: %s, Unique id: %d, ARGUS LCN: %d, ARGUS Id: %d, ARGUS GUID: %s\n",
              channel->Name().c_str(), tag.GetUniqueId(), tag.GetChannelNumber(), channel->ID(),
              channel->Guid().c_str());
        }
        else
        {
          m_RadioChannels.push_back(channel);
          kodi::Log(ADDON_LOG_DEBUG,
                    "Found Radio channel: %s, Unique id: %d, ARGUS LCN: %d, ARGUS Id: %d, ARGUS "
                    "GUID: %s\n",
                    channel->Name().c_str(), tag.GetUniqueId(), tag.GetChannelNumber(),
                    channel->ID(), channel->Guid().c_str());
        }
        results.Add(tag);
      }
    }

    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "RequestChannelList failed. Return value: %i\n", retval);
  }

  return PVR_ERROR_SERVER_ERROR;
}

/************************************************************/
/** Channel group handling **/

PVR_ERROR cPVRClientArgusTV::GetChannelGroupsAmount(int& amount)
{
  Json::Value response;
  amount = 0;
  if (m_rpc.RequestTVChannelGroups(response) >= 0)
    amount += response.size();
  if (m_rpc.RequestRadioChannelGroups(response) >= 0)
    amount += response.size();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetChannelGroups(bool radio,
                                              kodi::addon::PVRChannelGroupsResultSet& results)
{
  Json::Value response;
  int retval;

  if (radio && !m_base.GetSettings().RadioEnabled())
    return PVR_ERROR_NO_ERROR;

  if (!radio)
  {
    retval = m_rpc.RequestTVChannelGroups(response);
  }
  else
  {
    retval = m_rpc.RequestRadioChannelGroups(response);
  }
  if (retval >= 0)
  {
    int size = response.size();

    // parse channel group list
    for (int index = 0; index < size; ++index)
    {
      std::string name = response[index]["GroupName"].asString();
      std::string guid = response[index]["ChannelGroupId"].asString();
      int id = response[index]["Id"].asInt();
      if (!radio)
      {
        kodi::Log(ADDON_LOG_DEBUG, "Found TV channel group %s, ARGUS Id: %d, ARGUS GUID: %s\n",
                  name.c_str(), id, guid.c_str());
      }
      else
      {
        kodi::Log(ADDON_LOG_DEBUG, "Found Radio channel group %s, ARGUS Id: %d, ARGUS GUID: %s\n",
                  name.c_str(), id, guid.c_str());
      }
      kodi::addon::PVRChannelGroup tag;

      tag.SetIsRadio(radio);
      tag.SetPosition(0); // default ordering of the groups
      tag.SetGroupName(name);

      results.Add(tag);
    }
    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    return PVR_ERROR_SERVER_ERROR;
  }
}

PVR_ERROR cPVRClientArgusTV::GetChannelGroupMembers(
    const kodi::addon::PVRChannelGroup& group,
    kodi::addon::PVRChannelGroupMembersResultSet& results)
{
  Json::Value response;
  int retval;

  // Step 1, find the GUID for this channelgroup
  if (!group.GetIsRadio())
  {
    retval = m_rpc.RequestTVChannelGroups(response);
  }
  else
  {
    retval = m_rpc.RequestRadioChannelGroups(response);
  }
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "Could not get Channelgroups from server.");
    return PVR_ERROR_SERVER_ERROR;
  }

  std::string guid = "";
  std::string name = "";
  int size = response.size();
  for (int index = 0; index < size; ++index)
  {
    name = response[index]["GroupName"].asString();
    guid = response[index]["ChannelGroupId"].asString();
    if (name == group.GetGroupName())
      break;
  }
  if (name != group.GetGroupName())
  {
    kodi::Log(ADDON_LOG_ERROR,
              "Channelgroup %s was not found while trying to retrieve the channelgroup members.",
              group.GetGroupName().c_str());
    return PVR_ERROR_SERVER_ERROR;
  }

  // Step 2 use the guid to retrieve the list of member channels
  retval = m_rpc.RequestChannelGroupMembers(guid, response);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "Could not get members for Channelgroup \"%s\" (%s) from server.",
              name.c_str(), guid.c_str());
    return PVR_ERROR_SERVER_ERROR;
  }
  size = response.size();
  for (int index = 0; index < size; index++)
  {
    std::string channelId = response[index]["ChannelId"].asString();
    std::string channelName = response[index]["DisplayName"].asString();
    int id = response[index]["Id"].asInt();
    int lcn = response[index]["LogicalChannelNumber"].asInt();

    kodi::addon::PVRChannelGroupMember tag;

    tag.SetGroupName(group.GetGroupName());
    tag.SetChannelUniqueId(id);
    tag.SetChannelNumber(lcn);

    kodi::Log(ADDON_LOG_DEBUG, "%s - add channel %s (%d) to group '%s' ARGUS LCN: %d, ARGUS Id: %d",
              __FUNCTION__, channelName.c_str(), tag.GetChannelUniqueId(),
              tag.GetGroupName().c_str(), tag.GetChannelNumber(), id);

    results.Add(tag);
  }
  return PVR_ERROR_NO_ERROR;
}

/************************************************************/
/** Record handling **/

PVR_ERROR cPVRClientArgusTV::GetRecordingsAmount(bool deleted, int& amount)
{
  Json::Value response;
  int retval = -1;
  amount = 0;

  kodi::Log(ADDON_LOG_DEBUG, "GetNumRecordings()");
  retval = m_rpc.GetRecordingGroupByTitle(response);
  if (retval >= 0)
  {
    int size = response.size();

    // parse channelgroup list
    for (int index = 0; index < size; ++index)
    {
      cRecordingGroup recordinggroup;
      if (recordinggroup.Parse(response[index]))
      {
        amount += recordinggroup.RecordingsCount();
      }
    }

    return PVR_ERROR_NO_ERROR;
  }

  return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetRecordings(bool deleted,
                                           kodi::addon::PVRRecordingsResultSet& results)
{
  Json::Value recordinggroupresponse;
  int retval = -1;
  int iNumRecordings = 0;

  m_RecordingsMap.clear();

  kodi::Log(ADDON_LOG_DEBUG, "RequestRecordingsList()");
  auto startTime = std::chrono::system_clock::now();
  retval = m_rpc.GetRecordingGroupByTitle(recordinggroupresponse);
  if (retval >= 0)
  {
    // process list of recording groups
    int size = recordinggroupresponse.size();
    for (int recordinggroupindex = 0; recordinggroupindex < size; ++recordinggroupindex)
    {
      cRecordingGroup recordinggroup;
      if (recordinggroup.Parse(recordinggroupresponse[recordinggroupindex]))
      {
        Json::Value recordingsbytitleresponse;
        retval = m_rpc.GetFullRecordingsForTitle(recordinggroup.ProgramTitle(),
                                                 recordingsbytitleresponse);
        if (retval >= 0)
        {
          // process list of recording details for this group
          int nrOfRecordings = recordingsbytitleresponse.size();
          for (int recordingindex = 0; recordingindex < nrOfRecordings; recordingindex++)
          {
            cRecording recording;

            if (recording.Parse(recordingsbytitleresponse[recordingindex]))
            {
              kodi::addon::PVRRecording tag;

              //There may be cases where series and/or episode are populated withe 0 by default
              //if neither value is more than 0, there is no value to use or show them
              if (recording.SeriesNumber() > 0 || recording.EpisodeNumber() > 0)
              {
                tag.SetSeriesNumber(recording.SeriesNumber());
                tag.SetEpisodeNumber(recording.EpisodeNumber());
              }

              tag.SetRecordingId(recording.RecordingId());
              tag.SetChannelName(recording.ChannelDisplayName());
              tag.SetLifetime(MAXLIFETIME); //TODO: recording.Lifetime());
              tag.SetPriority(recording.SchedulePriority());
              tag.SetRecordingTime(recording.RecordingStartTime());
              tag.SetDuration(recording.RecordingStopTime() - recording.RecordingStartTime());
              tag.SetPlot(recording.Description());
              tag.SetPlayCount(recording.FullyWatchedCount());
              tag.SetLastPlayedPosition(recording.LastWatchedPosition());
              tag.SetTitle(recording.Title());
              tag.SetEpisodeName(recording.SubTitle());
              if (nrOfRecordings > 1 || m_base.GetSettings().UseFolder())
                tag.SetDirectory(recording.Title());

              m_RecordingsMap[tag.GetRecordingId()] = recording.RecordingFileName();

              /* TODO: PVR API 5.0.0: Implement this */
              tag.SetChannelUid(PVR_CHANNEL_INVALID_UID);

              /* TODO: PVR API 5.1.0: Implement this */
              tag.SetChannelType(PVR_RECORDING_CHANNEL_TYPE_UNKNOWN);

              results.Add(tag);
              iNumRecordings++;
            }
          }
        }
      }
    }
  }
  auto totalTime = std::chrono::system_clock::now() - startTime;
  kodi::Log(ADDON_LOG_INFO, "Retrieving %d recordings took %d milliseconds.", iNumRecordings,
            std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count());
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::DeleteRecording(const kodi::addon::PVRRecording& recinfo)
{
  PVR_ERROR rc = PVR_ERROR_FAILED;
  std::string UNCname;

  if (!FindRecEntryUNC(recinfo.GetRecordingId(), UNCname))
    return PVR_ERROR_FAILED;

  kodi::Log(ADDON_LOG_DEBUG, "->DeleteRecording(%s)", UNCname.c_str());

  kodi::Log(ADDON_LOG_DEBUG, "->DeleteRecording(%s == \"%s\")", recinfo.GetRecordingId().c_str(),
            UNCname.c_str());
  // JSONify the stream_url
  Json::Value recordingname(UNCname);
  Json::StreamWriterBuilder wbuilder;
  std::string jsonval = Json::writeString(wbuilder, recordingname);

  if (m_rpc.DeleteRecording(jsonval) >= 0)
  {
    // Trigger XBMC to update it's list
    kodi::addon::CInstancePVRClient::TriggerRecordingUpdate();
    rc = PVR_ERROR_NO_ERROR;
  }

  return rc;
}

PVR_ERROR cPVRClientArgusTV::RenameRecording(const kodi::addon::PVRRecording& recinfo)
{
  NOTUSED(recinfo);
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::SetRecordingLastPlayedPosition(
    const kodi::addon::PVRRecording& recinfo, int lastplayedposition)
{
  std::string recordingfilename;

  if (!FindRecEntryUNC(recinfo.GetRecordingId(), recordingfilename))
    return PVR_ERROR_FAILED;

  kodi::Log(ADDON_LOG_DEBUG, "->SetRecordingLastPlayedPosition(index=%s [%s], %d)",
            recinfo.GetRecordingId().c_str(), recordingfilename.c_str(), lastplayedposition);

  // JSONify the stream_url
  Json::Value recordingname(recordingfilename);
  Json::StreamWriterBuilder wbuilder;
  std::string jsonval = Json::writeString(wbuilder, recordingname);
  int retval = m_rpc.SetRecordingLastWatchedPosition(jsonval, lastplayedposition);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_INFO, "Failed to set recording last watched position (%d)", retval);
    return PVR_ERROR_SERVER_ERROR;
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetRecordingLastPlayedPosition(
    const kodi::addon::PVRRecording& recinfo, int& position)
{
  std::string recordingfilename;

  if (!FindRecEntryUNC(recinfo.GetRecordingId(), recordingfilename))
    return PVR_ERROR_SERVER_ERROR;

  kodi::Log(ADDON_LOG_DEBUG, "->GetRecordingLastPlayedPosition(index=%s [%s])",
            recinfo.GetRecordingId().c_str(), recordingfilename.c_str());

  // JSONify the stream_url
  Json::Value response;
  Json::Value recordingname(recordingfilename);
  Json::StreamWriterBuilder wbuilder;
  std::string jsonval = Json::writeString(wbuilder, recordingname);
  int retval = m_rpc.GetRecordingLastWatchedPosition(jsonval, response);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_INFO, "Failed to get recording last watched position (%d)", retval);
    return PVR_ERROR_SERVER_ERROR;
  }

  position = response.asInt();
  kodi::Log(ADDON_LOG_DEBUG, "GetRecordingLastPlayedPosition(index=%s [%s]) returns %d.\n",
            recinfo.GetRecordingId().c_str(), recordingfilename.c_str(), retval);

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::SetRecordingPlayCount(const kodi::addon::PVRRecording& recinfo,
                                                   int playcount)
{
  std::string recordingfilename;

  if (!FindRecEntryUNC(recinfo.GetRecordingId(), recordingfilename))
    return PVR_ERROR_FAILED;

  kodi::Log(ADDON_LOG_DEBUG, "->SetRecordingPlayCount(index=%s [%s], %d)",
            recinfo.GetRecordingId().c_str(), recordingfilename.c_str(), playcount);

  // JSONify the stream_url
  Json::Value recordingname(recordingfilename);
  Json::StreamWriterBuilder wbuilder;
  std::string jsonval = Json::writeString(wbuilder, recordingname);
  int retval = m_rpc.SetRecordingFullyWatchedCount(jsonval, playcount);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_INFO, "Failed to set recording play count (%d)", retval);
    return PVR_ERROR_SERVER_ERROR;
  }

  return PVR_ERROR_NO_ERROR;
}

// GetRecordingEdl source borrowed from pvr.wmc
PVR_ERROR cPVRClientArgusTV::GetRecordingEdl(const kodi::addon::PVRRecording& recording,
                                             std::vector<kodi::addon::PVREDLEntry>& edl)
{
  std::string streamFileName; // the name of the stream file
  if (!FindRecEntry(recording.GetRecordingId(), streamFileName))
    return PVR_ERROR_SERVER_ERROR;

  if (!streamFileName.empty()) // read the edl for the current stream file
  {
    // see if edl file for currently streaming recording exists
    std::string theEdlFile = streamFileName;
    // swap .wtv extension for .edl
    std::string::size_type result = theEdlFile.find_last_of('.');
    if (std::string::npos != result)
      theEdlFile.erase(result);
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "File extender error: '%s'", theEdlFile.c_str());
      return PVR_ERROR_FAILED;
    }
    theEdlFile.append(".edl");

    kodi::Log(ADDON_LOG_DEBUG, "Opening EDL file: '%s'", theEdlFile.c_str());

    kodi::vfs::CFile fileHandle;
    if (fileHandle.OpenFile(theEdlFile))
    {
      std::string svals;
      while (fileHandle.ReadLine(svals))
      {
        size_t nidx = svals.find_last_not_of("\r");
        svals.erase(svals.npos == nidx ? 0 : ++nidx); // trim windows /r if its there

        std::vector<std::string> vals = kodi::tools::StringUtils::Split(svals, "\t"); // split on tabs
        if (vals.size() == 3)
        {
          kodi::addon::PVREDLEntry entry;
          entry.SetStart(static_cast<int64_t>(std::strtod(vals[0].c_str(), nullptr) *
                                              1000)); // convert s to ms
          entry.SetEnd(static_cast<int64_t>(std::strtod(vals[1].c_str(), nullptr) * 1000));
          entry.SetType(PVR_EDL_TYPE(atoi(vals[2].c_str())));
          edl.emplace_back(entry);
        }
      }
      if (!edl.empty())
        kodi::Log(ADDON_LOG_DEBUG, "EDL data found.");
      else
        kodi::Log(ADDON_LOG_DEBUG, "No EDL data found.");
      return PVR_ERROR_NO_ERROR;
    }
    else
      kodi::Log(ADDON_LOG_DEBUG, "No EDL file found.");
  }
  return PVR_ERROR_FAILED;
}


/************************************************************/
/** Timer handling */

PVR_ERROR cPVRClientArgusTV::GetTimersAmount(int& amount)
{
  // Not directly possible in ARGUS TV
  Json::Value response;

  kodi::Log(ADDON_LOG_DEBUG, "GetNumTimers()");
  // pick up the schedulelist for TV
  int retval = m_rpc.GetUpcomingRecordings(response);
  if (retval < 0)
  {
    return PVR_ERROR_SERVER_ERROR;
  }

  amount = response.size();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::GetTimers(kodi::addon::PVRTimersResultSet& results)
{
  Json::Value activeRecordingsResponse, upcomingRecordingsResponse;
  int iNumberOfTimers = 0;
  int numberoftimers;

  kodi::Log(ADDON_LOG_DEBUG, "%s", __FUNCTION__);

  // retrieve the currently active recordings
  int retval = m_rpc.GetActiveRecordings(activeRecordingsResponse);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "Unable to retrieve active recordings from server.");
    return PVR_ERROR_SERVER_ERROR;
  }

  // pick up the upcoming recordings
  retval = m_rpc.GetUpcomingRecordings(upcomingRecordingsResponse);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "Unable to retrieve upcoming programs from server.");
    return PVR_ERROR_SERVER_ERROR;
  }

  numberoftimers = upcomingRecordingsResponse.size();

  for (int i = 0; i < numberoftimers; i++)
  {
    cUpcomingRecording upcomingrecording;
    if (upcomingrecording.Parse(upcomingRecordingsResponse[i]))
    {
      kodi::addon::PVRTimer tag;

      /* TODO: Implement own timer types to get support for the timer features introduced with PVR API 1.9.7 */
      tag.SetTimerType(PVR_TIMER_TYPE_NONE);

      tag.SetClientIndex(upcomingrecording.ID());
      tag.SetClientChannelUid(upcomingrecording.ChannelID());
      tag.SetStartTime(upcomingrecording.StartTime());
      tag.SetEndTime(upcomingrecording.StopTime());

      // build the XBMC PVR State
      if (upcomingrecording.IsCancelled())
      {
        tag.SetState(PVR_TIMER_STATE_CANCELLED);
      }
      else if (upcomingrecording.IsInConflict())
      {
        if (upcomingrecording.IsAllocated())
          tag.SetState(PVR_TIMER_STATE_CONFLICT_OK);
        else
          tag.SetState(PVR_TIMER_STATE_CONFLICT_NOK);
      }
      else if (!upcomingrecording.IsAllocated())
      {
        //not allocated --> won't be recorded
        tag.SetState(PVR_TIMER_STATE_ERROR);
      }
      else
      {
        tag.SetState(PVR_TIMER_STATE_SCHEDULED);
      }

      if (tag.GetState() == PVR_TIMER_STATE_SCHEDULED ||
          tag.GetState() == PVR_TIMER_STATE_CONFLICT_OK) //check if they are currently recording
      {
        if (activeRecordingsResponse.size() > 0)
        {
          // Is the this upcoming recording in the list of active recordings?
          for (Json::Value::UInt j = 0; j < activeRecordingsResponse.size(); j++)
          {
            cActiveRecording activerecording;
            if (activerecording.Parse(activeRecordingsResponse[j]))
            {
              if (upcomingrecording.UpcomingProgramId() == activerecording.UpcomingProgramId())
              {
                tag.SetState(PVR_TIMER_STATE_RECORDING);
                break;
              }
            }
          }
        }
      }

      tag.SetTitle(upcomingrecording.Title());
      tag.SetDirectory("");
      tag.SetSummary("");
      tag.SetPriority(0);
      tag.SetLifetime(0);
      tag.SetFirstDay(0);
      tag.SetWeekdays(0);
      tag.SetEPGUid(0);
      tag.SetMarginStart(upcomingrecording.PreRecordSeconds() / 60);
      tag.SetMarginEnd(upcomingrecording.PostRecordSeconds() / 60);
      tag.SetGenreType(0);
      tag.SetGenreSubType(0);

      results.Add(tag);

      kodi::Log(ADDON_LOG_DEBUG,
                "Found timer: %s, Unique id: %d, ARGUS ProgramId: %d, ARGUS ChannelId: %d\n",
                tag.GetTitle().c_str(), tag.GetClientIndex(), upcomingrecording.ID(),
                upcomingrecording.ChannelID());
      iNumberOfTimers++;
    }
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::AddTimer(const kodi::addon::PVRTimer& timerinfo)
{
  kodi::Log(ADDON_LOG_DEBUG, "AddTimer(title %s, start @ %d, end @ %d)",
            timerinfo.GetTitle().c_str(), timerinfo.GetStartTime(), timerinfo.GetEndTime());

  // re-synthesize the ARGUS TV channel GUID
  cChannel* pChannel = FetchChannel(timerinfo.GetClientChannelUid());
  if (pChannel == nullptr)
  {
    kodi::Log(ADDON_LOG_ERROR,
              "Unable to translate XBMC channel %d to ARGUS TV channel GUID, timer not added.",
              timerinfo.GetClientChannelUid());
    kodi::QueueNotification(QUEUE_ERROR, "", "Can't map XBMC Channel to ARGUS");
    return PVR_ERROR_SERVER_ERROR;
  }

  kodi::Log(ADDON_LOG_DEBUG, "%s: XBMC channel %d translated to ARGUS channel %s.", __FUNCTION__,
            timerinfo.GetClientChannelUid(), pChannel->Guid().c_str());

  // Try to get original EPG data from ARGUS
  time_t startTime = timerinfo.GetStartTime();
  struct tm* tm_start = localtime(&startTime);
  time_t endTime = timerinfo.GetEndTime();
  struct tm* tm_end = localtime(&endTime);

  Json::Value epgResponse;
  kodi::Log(ADDON_LOG_DEBUG, "%s: Getting EPG Data for ARGUS TV channel %s", __FUNCTION__,
            pChannel->GuideChannelID().c_str());
  int retval = m_rpc.GetEPGData(pChannel->GuideChannelID(), *tm_start, *tm_end, epgResponse);

  std::string programTitle = timerinfo.GetTitle();
  if (retval >= 0)
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s: Getting EPG Data for ARGUS TV channel %s returned %d entries.",
              __FUNCTION__, pChannel->GuideChannelID().c_str(), epgResponse.size());
    if (epgResponse.size() > 0)
    {
      programTitle = epgResponse[0u]["Title"].asString();
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s: Getting EPG Data for ARGUS TV channel %s failed.", __FUNCTION__,
              pChannel->GuideChannelID().c_str());
  }

  Json::Value addScheduleResponse;
  time_t starttime = timerinfo.GetStartTime();
  if (starttime == 0)
    starttime = time(nullptr);
  retval = m_rpc.AddOneTimeSchedule(pChannel->Guid(), starttime, programTitle,
                                    timerinfo.GetMarginStart() * 60, timerinfo.GetMarginEnd() * 60,
                                    timerinfo.GetLifetime(), addScheduleResponse);
  if (retval < 0)
  {
    return PVR_ERROR_SERVER_ERROR;
  }

  std::string scheduleid = addScheduleResponse["ScheduleId"].asString();

  kodi::Log(ADDON_LOG_DEBUG, "%s: ARGUS one-time schedule added with id %s.", __FUNCTION__,
            scheduleid.c_str());


  // Ok, we created a schedule, but did that lead to an upcoming recording?
  Json::Value upcomingProgramsResponse;
  retval = m_rpc.GetUpcomingProgramsForSchedule(addScheduleResponse, upcomingProgramsResponse);

  // We should have at least one upcoming program for this schedule, otherwise nothing will be recorded
  if (retval <= 0)
  {
    kodi::Log(ADDON_LOG_INFO, "The new schedule does not lead to an upcoming program, removing "
                              "schedule and adding a manual one.");
    // remove the added (now stale) schedule, ignore failure (what are we to do anyway?)
    m_rpc.DeleteSchedule(scheduleid);

    // Okay, add a manual schedule (forced recording) but now we need to add pre- and post-recording ourselves
    time_t manualStartTime = starttime - (timerinfo.GetMarginStart() * 60);
    time_t manualEndTime = timerinfo.GetEndTime() + (timerinfo.GetMarginEnd() * 60);
    retval = m_rpc.AddManualSchedule(pChannel->Guid(), manualStartTime,
                                     manualEndTime - manualStartTime, timerinfo.GetTitle().c_str(),
                                     timerinfo.GetMarginStart() * 60, timerinfo.GetMarginEnd() * 60,
                                     timerinfo.GetLifetime(), addScheduleResponse);
    if (retval < 0)
    {
      kodi::Log(ADDON_LOG_ERROR, "A manual schedule could not be added.");
      return PVR_ERROR_SERVER_ERROR;
    }
  }

  // Trigger an update of the PVR timers
  kodi::addon::CInstancePVRClient::TriggerTimerUpdate();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR cPVRClientArgusTV::DeleteTimer(const kodi::addon::PVRTimer& timerinfo, bool force)
{
  NOTUSED(force);
  Json::Value upcomingProgramsResponse, activeRecordingsResponse;

  kodi::Log(ADDON_LOG_DEBUG, "DeleteTimer()");

  // retrieve the currently active recordings
  int retval = m_rpc.GetActiveRecordings(activeRecordingsResponse);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "Unable to retrieve active recordings from server.");
    return PVR_ERROR_SERVER_ERROR;
  }

  // pick up the upcoming recordings
  retval = m_rpc.GetUpcomingRecordings(upcomingProgramsResponse);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "Unable to retrieve upcoming programs from server.");
    return PVR_ERROR_SERVER_ERROR;
  }

  // try to find the upcoming recording that matches this xbmc timer
  int numberoftimers = upcomingProgramsResponse.size();
  for (int i = 0; i < numberoftimers; i++)
  {
    cUpcomingRecording upcomingrecording;
    if (upcomingrecording.Parse(upcomingProgramsResponse[i]))
    {
      if (upcomingrecording.ID() == (int)timerinfo.GetClientIndex())
      {
        // Okay, we matched the timer to an upcoming program, but is it recording right now?
        if (activeRecordingsResponse.size() > 0)
        {
          // Is the this upcoming program in the list of active recordings?
          for (Json::Value::UInt j = 0; j < activeRecordingsResponse.size(); j++)
          {
            cActiveRecording activerecording;
            if (activerecording.Parse(activeRecordingsResponse[j]))
            {
              if (upcomingrecording.UpcomingProgramId() == activerecording.UpcomingProgramId())
              {
                // Abort this recording
                retval = m_rpc.AbortActiveRecording(activeRecordingsResponse[j]);
                if (retval != 0)
                {
                  kodi::Log(ADDON_LOG_ERROR,
                            "Unable to cancel the active recording of \"%s\" on the server. Will "
                            "try to cancel the program.",
                            upcomingrecording.Title().c_str());
                }
                break;
              }
            }
          }
        }

        Json::Value scheduleResponse;
        retval = m_rpc.GetScheduleById(upcomingrecording.ScheduleId(), scheduleResponse);
        std::string schedulename = scheduleResponse["Name"].asString();

        if (scheduleResponse["IsOneTime"].asBool() == true)
        {
          retval = m_rpc.DeleteSchedule(upcomingrecording.ScheduleId());
          if (retval < 0)
          {
            kodi::Log(ADDON_LOG_INFO, "Unable to delete schedule %s from server.",
                      schedulename.c_str());
            return PVR_ERROR_SERVER_ERROR;
          }
        }
        else
        {
          retval = m_rpc.CancelUpcomingProgram(
              upcomingrecording.ScheduleId(), upcomingrecording.ChannelId(),
              upcomingrecording.StartTime(), upcomingrecording.GuideProgramId());
          if (retval < 0)
          {
            kodi::Log(ADDON_LOG_ERROR, "Unable to cancel upcoming program from server.");
            return PVR_ERROR_SERVER_ERROR;
          }
        }

        // Trigger an update of the PVR timers
        kodi::addon::CInstancePVRClient::TriggerTimerUpdate();
        return PVR_ERROR_NO_ERROR;
      }
    }
  }
  return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR cPVRClientArgusTV::UpdateTimer(const kodi::addon::PVRTimer& timerinfo)
{
  NOTUSED(timerinfo);
  return PVR_ERROR_NOT_IMPLEMENTED;
}


/************************************************************/
/** Live stream handling */
cChannel* cPVRClientArgusTV::FetchChannel(int channelid, bool LogError)
{
  std::lock_guard<std::mutex> lock(m_ChannelCacheMutex);
  cChannel* rc = FetchChannel(m_TVChannels, channelid, false);
  if (rc == nullptr)
    rc = FetchChannel(m_RadioChannels, channelid, false);

  if (LogError && rc == nullptr)
    kodi::Log(ADDON_LOG_ERROR, "XBMC channel with id %d not found in the channel caches!.",
              channelid);
  return rc;
}

cChannel* cPVRClientArgusTV::FetchChannel(std::vector<cChannel*> m_Channels,
                                          int channelid,
                                          bool LogError)
{
  // Search for this channel in our local channel list to find the original ChannelID back:
  std::vector<cChannel*>::iterator it;

  for (it = m_Channels.begin(); it < m_Channels.end(); it++)
  {
    if ((*it)->ID() == channelid)
    {
      return *it;
    }
  }

  if (LogError)
    kodi::Log(ADDON_LOG_ERROR, "XBMC channel with id %d not found in the channel cache!.",
              channelid);
  return nullptr;
}

void cPVRClientArgusTV::FreeChannels(std::vector<cChannel*> m_Channels)
{
  // Search for this channel in our local channel list to find the original ChannelID back:
  std::vector<cChannel*>::iterator it;

  for (it = m_Channels.begin(); it < m_Channels.end(); it++)
  {
    SafeDelete(*it);
  }
}

bool cPVRClientArgusTV::_OpenLiveStream(const kodi::addon::PVRChannel& channelinfo)
{
  kodi::Log(ADDON_LOG_DEBUG, "->_OpenLiveStream(%i)", channelinfo.GetUniqueId());

  if (((int)channelinfo.GetUniqueId()) == m_iCurrentChannel)
  {
    kodi::Log(ADDON_LOG_INFO,
              "New channel uid equal to the already streaming channel. Skipping re-tune.");
    return true;
  }

  m_iCurrentChannel =
      -1; // make sure that it is not a valid channel nr in case it will fail lateron

  cChannel* channel = FetchChannel(channelinfo.GetUniqueId());

  if (channel)
  {
    std::string filename;
    kodi::Log(ADDON_LOG_INFO, "Tune XBMC channel: %i", channelinfo.GetUniqueId());
    kodi::Log(ADDON_LOG_INFO, "Corresponding ARGUS TV channel: %s", channel->Guid().c_str());

    int retval = m_rpc.TuneLiveStream(channel->Guid(), channel->Type(), channel->Name(), filename);
    if (retval == m_rpc.NoReTunePossible)
    {
      // Ok, we can't re-tune with the current live stream still running
      // So stop it and re-try
      CloseLiveStream();
      kodi::Log(ADDON_LOG_INFO, "Re-Tune XBMC channel: %i", channelinfo.GetUniqueId());
      retval = m_rpc.TuneLiveStream(channel->Guid(), channel->Type(), channel->Name(), filename);
    }

    if (retval != E_SUCCESS)
    {
      switch (retval)
      {
        case CArgusTV::NoFreeCardFound:
          kodi::Log(ADDON_LOG_INFO, "No free tuner found.");
          kodi::QueueNotification(QUEUE_ERROR, "", "No free tuner found!");
          return false;
        case CArgusTV::IsScrambled:
          kodi::Log(ADDON_LOG_INFO, "Scrambled channel.");
          kodi::QueueNotification(QUEUE_ERROR, "", "Scrambled channel!");
          return false;
        case CArgusTV::ChannelTuneFailed:
          kodi::Log(ADDON_LOG_INFO, "Tuning failed.");
          kodi::QueueNotification(QUEUE_ERROR, "", "Tuning failed!");
          return false;
        default:
          kodi::Log(ADDON_LOG_ERROR, "Tuning failed, unknown error");
          kodi::QueueNotification(QUEUE_ERROR, "", "Unknown error!");
          return false;
      }
    }

    filename = ToCIFS(filename);

    InsertUser(m_base, filename);

    if (retval != E_SUCCESS || filename.length() == 0)
    {
      kodi::Log(ADDON_LOG_ERROR, "Could not start the timeshift for channel %i (%s)",
                channelinfo.GetUniqueId(), channel->Guid().c_str());
      CloseLiveStream();
      return false;
    }

    // reset the signal quality poll interval after tuning
    m_signalqualityInterval = 0;

    kodi::Log(ADDON_LOG_INFO, "Live stream file: %s", filename.c_str());
    m_bTimeShiftStarted = true;
    m_iCurrentChannel = channelinfo.GetUniqueId();
    m_keepalive->StartThread();

#if defined(ATV_DUMPTS)
    if (ofd != -1)
      close(ofd);
    strncpy(ofn, "/tmp/atv.XXXXXX", sizeof(ofn));
    if ((ofd = mkostemp(ofn, O_CREAT | O_TRUNC)) == -1)
    {
      kodi::Log(ADDON_LOG_ERROR, "couldn't open dumpfile %s (error %d: %s).", ofn, errno,
                strerror(errno));
    }
    else
    {
      kodi::Log(ADDON_LOG_INFO, "opened dumpfile %s.", ofn);
    }
#endif

    if (m_tsreader != nullptr)
    {
      //kodi::Log(ADDON_LOG_DEBUG, "Re-using existing TsReader...");
      //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
      //m_tsreader->OnZap();
      kodi::Log(ADDON_LOG_DEBUG, "Close existing and open new TsReader...");
      m_tsreader->Close();
      SafeDelete(m_tsreader);
    }
    // Open Timeshift buffer
    // TODO: rtsp support
    m_tsreader = new CTsReader();
    kodi::Log(ADDON_LOG_DEBUG, "Open TsReader");
    m_tsreader->Open(filename.c_str());
    m_tsreader->OnZap();
    kodi::Log(ADDON_LOG_DEBUG, "Delaying %ld milliseconds.", m_base.GetSettings().TuneDelay());
    std::this_thread::sleep_for(std::chrono::milliseconds(m_base.GetSettings().TuneDelay()));
    return true;
  }
  else
  {
    kodi::Log(ADDON_LOG_ERROR, "Could not get ARGUS TV channel guid for channel %i.",
              channelinfo.GetUniqueId());
    kodi::QueueNotification(QUEUE_ERROR, "", "XBMC Channel to GUID");
  }

  CloseLiveStream();
  return false;
}

bool cPVRClientArgusTV::OpenLiveStream(const kodi::addon::PVRChannel& channelinfo)
{
  auto startTime = std::chrono::system_clock::now();
  bool rc = _OpenLiveStream(channelinfo);
  auto totalTime = std::chrono::system_clock::now() - startTime;
  kodi::Log(ADDON_LOG_INFO, "Opening live stream took %d milliseconds.",
            std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count());
  return rc;
}

int cPVRClientArgusTV::ReadLiveStream(unsigned char* pBuffer, unsigned int iBufferSize)
{
  unsigned long read_wanted = iBufferSize;
  unsigned long read_done = 0;
  static int read_timeouts = 0;
  unsigned char* bufptr = pBuffer;

  // kodi::Log(ADDON_LOG_DEBUG, "->ReadLiveStream(buf_size=%i)", iBufferSize);
  if (!m_tsreader)
    return -1;

  while (read_done < (unsigned long)iBufferSize)
  {
    read_wanted = iBufferSize - read_done;

    long lRc = 0;
    if ((lRc = m_tsreader->Read(bufptr, read_wanted, &read_wanted)) > 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(400));
      read_timeouts++;
      kodi::Log(ADDON_LOG_INFO, "ReadLiveStream requested %d but only read %d bytes.", iBufferSize,
                read_wanted);
      return read_wanted;
    }
    read_done += read_wanted;

    if (read_done < (unsigned long)iBufferSize)
    {
      if (read_timeouts > 25)
      {
        kodi::Log(ADDON_LOG_INFO, "No data in 1 second");
        read_timeouts = 0;
        return read_done;
      }
      bufptr += read_wanted;
      read_timeouts++;
      std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
  }
#if defined(ATV_DUMPTS)
  if (write(ofd, pBuffer, read_done) < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "couldn't write %d bytes to dumpfile %s (error %d: %s).", read_done,
              ofn, errno, strerror(errno));
  }
#endif
  // kodi::Log(ADDON_LOG_DEBUG, "ReadLiveStream(buf_size=%i), %d timeouts", iBufferSize, read_timeouts);
  read_timeouts = 0;
  return read_done;
}

int64_t cPVRClientArgusTV::SeekLiveStream(int64_t pos, int whence)
{
  static std::string zz[] = {"Begin", "Current", "End"};
  kodi::Log(ADDON_LOG_DEBUG, "SeekLiveStream (%lld, %s).", pos, zz[whence].c_str());
  if (!m_tsreader)
  {
    return -1;
  }
  return m_tsreader->SetFilePointer(pos, whence);
}

int64_t cPVRClientArgusTV::LengthLiveStream()
{
  if (!m_tsreader)
  {
    return -1;
  }
  return m_tsreader->GetFileSize();
}


void cPVRClientArgusTV::CloseLiveStream()
{
  std::string result;
  kodi::Log(ADDON_LOG_INFO, "CloseLiveStream");

  m_keepalive->StopThread();

#if defined(ATV_DUMPTS)
  if (ofd != -1)
  {
    if (close(ofd) == -1)
    {
      kodi::Log(ADDON_LOG_ERROR, "couldn't close dumpfile %s (error %d: %s).", ofn, errno,
                strerror(errno));
    }
    ofd = -1;
  }
#endif

  if (m_bTimeShiftStarted)
  {
    if (m_tsreader)
    {
      kodi::Log(ADDON_LOG_DEBUG, "Close TsReader");
      m_tsreader->Close();
#if defined(TARGET_WINDOWS)
      kodi::Log(ADDON_LOG_DEBUG, "ReadLiveStream: %I64d calls took %I64d nanoseconds.",
                m_tsreader->sigmaCount(), m_tsreader->sigmaTime());
#endif
      SafeDelete(m_tsreader);
    }
    m_rpc.StopLiveStream();
    m_bTimeShiftStarted = false;
    m_iCurrentChannel = -1;
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "CloseLiveStream: Nothing to do.");
  }
}

PVR_ERROR cPVRClientArgusTV::GetSignalStatus(int channelUid,
                                             kodi::addon::PVRSignalStatus& signalStatus)
{
  static kodi::addon::PVRSignalStatus tag;

  // Only do the REST call once out of N
  if (m_signalqualityInterval-- <= 0)
  {
    m_signalqualityInterval = SIGNALQUALITY_INTERVAL;
    Json::Value response;
    m_rpc.SignalQuality(response);

    std::string cardtype = "";
    switch (response["CardType"].asInt())
    {
      case 0x80:
        cardtype = "Analog";
        break;
      case 8:
        cardtype = "ATSC";
        break;
      case 4:
        cardtype = "DVB-C";
        break;
      case 0x10:
        cardtype = "DVB-IP";
        break;
      case 1:
        cardtype = "DVB-S";
        break;
      case 2:
        cardtype = "DVB-T";
        break;
      default:
        cardtype = "Unknown card type";
        break;
    }
    tag.SetAdapterName("Provider" + response["ProviderName"].asString() + ", " + cardtype);
    tag.SetAdapterStatus(response["Name"].asString() + ", " +
                         (response["IsFreeToAir"].asBool() ? "free to air" : "encrypted"));
    tag.SetSNR((int)(response["SignalQuality"].asInt() * 655.35));
    tag.SetSignal((int)(response["SignalStrength"].asInt() * 655.35));
  }

  signalStatus = tag;

  return PVR_ERROR_NO_ERROR;
}

bool cPVRClientArgusTV::FindRecEntryUNC(const std::string& recId, std::string& recEntryURL)
{
  auto iter = m_RecordingsMap.find(recId);
  if (iter == m_RecordingsMap.end())
    return false;

  recEntryURL = ToUNC(iter->second);
  if (recEntryURL == "")
    return false;

  return true;
}

bool cPVRClientArgusTV::FindRecEntry(const std::string& recId, std::string& recEntryURL)
{
  auto iter = m_RecordingsMap.find(recId);
  if (iter == m_RecordingsMap.end())
    return false;

  recEntryURL = iter->second;
  InsertUser(m_base, recEntryURL);

  return !recEntryURL.empty();
}

/************************************************************/
/** Record stream handling */
bool cPVRClientArgusTV::OpenRecordedStream(const kodi::addon::PVRRecording& recinfo)
{
  std::string UNCname;

  if (!FindRecEntry(recinfo.GetRecordingId(), UNCname))
    return false;

  kodi::Log(ADDON_LOG_DEBUG, "->OpenRecordedStream(%s)", UNCname.c_str());

  if (m_tsreader != nullptr)
  {
    kodi::Log(ADDON_LOG_DEBUG, "Close existing TsReader...");
    m_tsreader->Close();
    SafeDelete(m_tsreader);
  }
  m_tsreader = new CTsReader();
  if (m_tsreader->Open(UNCname.c_str()) != S_OK)
  {
    SafeDelete(m_tsreader);
    return false;
  }

  m_bRecordingPlayback = true;

  return true;
}

void cPVRClientArgusTV::CloseRecordedStream(void)
{
  kodi::Log(ADDON_LOG_DEBUG, "->CloseRecordedStream()");

  m_bRecordingPlayback = false;

  if (m_tsreader)
  {
    kodi::Log(ADDON_LOG_DEBUG, "Close TsReader");
    m_tsreader->Close();
    SafeDelete(m_tsreader);
  }
}

int cPVRClientArgusTV::ReadRecordedStream(unsigned char* pBuffer, unsigned int iBuffersize)
{
  unsigned long read_done = 0;

  // kodi::Log(ADDON_LOG_DEBUG, "->ReadRecordedStream(buf_size=%i)", iBufferSize);
  if (!m_tsreader)
    return -1;

  long lRc = 0;
  if ((lRc = m_tsreader->Read(pBuffer, iBuffersize, &read_done)) > 0)
  {
    kodi::Log(ADDON_LOG_INFO, "ReadRecordedStream requested %d but only read %d bytes.",
              iBuffersize, read_done);
  }
  return read_done;
}

int64_t cPVRClientArgusTV::SeekRecordedStream(int64_t iPosition, int iWhence)
{
  if (!m_tsreader)
  {
    return -1;
  }
  if (iPosition == 0 && iWhence == SEEK_CUR)
  {
    return m_tsreader->GetFilePointer();
  }
  return m_tsreader->SetFilePointer(iPosition, iWhence);
}

int64_t cPVRClientArgusTV::LengthRecordedStream(void)
{
  if (!m_tsreader)
  {
    return -1;
  }
  return m_tsreader->GetFileSize();
}

/*
 * \brief Request the stream URL for live tv/live radio.
 */
const char* cPVRClientArgusTV::GetLiveStreamURL(const kodi::addon::PVRChannel& channelinfo)
{
  kodi::Log(ADDON_LOG_DEBUG, "->GetLiveStreamURL(%i)", channelinfo.GetUniqueId());
  bool rc = _OpenLiveStream(channelinfo);
  if (rc)
  {
    m_bTimeShiftStarted = true;
  }
  // sigh, the only reason to use a class member here is to have storage for the const char *
  // pointing to the std::string when this method returns (and locals would go out of scope)
  m_PlaybackURL = m_rpc.GetLiveStreamURL();
  kodi::Log(ADDON_LOG_DEBUG, "<-GetLiveStreamURL returns URL(%s)", m_PlaybackURL.c_str());
  return m_PlaybackURL.c_str();
}

bool cPVRClientArgusTV::CanSeekStream()
{
  bool rc = (m_tsreader != nullptr);
  kodi::Log(ADDON_LOG_DEBUG, "<-CanSeekStream returns %s", rc ? "true" : "false");
  return rc;
}

bool cPVRClientArgusTV::CanPauseStream()
{
  bool rc = (m_tsreader != nullptr);
  kodi::Log(ADDON_LOG_DEBUG, "<-CanPauseStream returns %s", rc ? "true" : "false");
  return rc;
}
