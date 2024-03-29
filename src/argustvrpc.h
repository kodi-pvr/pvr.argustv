/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010-2012 Marcel Groothuis, Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <cstdlib>
#include <json/json.h>
#include <kodi/AddonBase.h>
#include <mutex>
#include <string>

#define ATV_2_2_0 (60)
#define ATV_REST_MINIMUM_API_VERSION ATV_2_2_0
#define ATV_REST_MAXIMUM_API_VERSION ATV_2_2_0

#define E_SUCCESS 0
#define E_FAILED -1
#define E_EMPTYRESPONSE -2

class ATTR_DLL_LOCAL CArgusTV
{
public:
  enum ChannelType
  {
    Television = 0,
    Radio = 1
  };

  enum RecordingGroupMode
  {
    GroupByProgramTitle = 0,
    GroupBySchedule = 1,
    GroupByCategory = 2,
    GroupByChannel = 3,
    GroupByRecordingDay = 4
  };

  enum SchedulePriority
  {
    VeryLow = 0,
    Low = 1,
    Normal = 2,
    High = 3,
    VeryHigh = 4
  };

  enum ScheduleType
  {
    Recording = 82, // 'R'
    Suggestion = 83, // 'S'
    Alert = 45 // 'A'
  };


  enum KeepUntilMode
  {
    UntilSpaceIsNeeded = 0,
    Forever = 1,
    NumberOfDays = 2,
    NumberOfEpisodes = 3
  };

  enum VideoAspectRatio
  {
    Unknown = 0,
    Standard = 1,
    Widescreen = 2
  };

  enum LiveStreamResult
  {
    Succeed = 0,
    NoFreeCardFound = 1,
    ChannelTuneFailed = 2,
    NoReTunePossible = 3,
    IsScrambled = 4,
    UnknownError = 98,
    NotSupported = 99
  };

  enum ServiceEventGroups
  {
    SystemEvents = 0x01,
    GuideEvents = 0x02,
    ScheduleEvents = 0x04,
    RecordingEvents = 0x08,
    AllEvents = 0x0F
  };

  /**
   * \brief Do some internal housekeeping at the start
   */
  void Initialize(const std::string& baseURL);

  /**
   * \brief Send a REST command to ARGUS and return the JSON response string
   * \param command       The command string url (starting from "ArgusTV/")
   * \param json_response Reference to a std::string used to store the json response string
   * \return 0 on ok, -1 on a failure
   */
  int ArgusTVRPC(const std::string& command,
                 const std::string& arguments,
                 std::string& json_response);

  /**
   * \brief Send a REST command to ARGUS and return the JSON response
   * \param command       The command string url (starting from "ArgusTV/")
   * \param json_response Reference to a Json::Value used to store the parsed Json value
   * \return 0 on ok, -1 on a failure
   */
  int ArgusTVJSONRPC(const std::string& command,
                     const std::string& arguments,
                     Json::Value& json_response);

  /**
   * \brief Send a REST command to ARGUS, write the response to a file and return the filename
   * \param command       The command string url (starting from "ArgusTV/")
   * \param newfilename   Reference to a std::string used to store the output file name
   * \param htt_presponse Reference to a long used to store the HTTP response code
   * \return 0 on ok, -1 on a failure
   */
  int ArgusTVRPCToFile(const std::string& command,
                       const std::string& arguments,
                       std::string& newfilename,
                       long& http_response);

  /**
   * \brief Ping core service.
   * \param requestedApiVersion  The API version the client needs, pass in Constants.ArgusTVRestApiVersion.
   * \return  0 if client and server are compatible, -1 if the client is too old, +1 if the client is newer than the server and -2 if the connection failed (server down?)
   */
  int Ping(int requestedApiVersion);

  /**
   * \brief Returns information (free disk space) from all recording disks.
   */
  int GetRecordingDisksInfo(Json::Value& response);

  /**
   * \brief Returns version information (for display only)
   */
  int GetDisplayVersion(Json::Value& response);

  /**
   * \brief GetPluginServices Get all configured plugin services. {activeOnly} = Set to true to only receive active plugins.
   * \brief Returns an array containing zero or more plugin services.
   * \param activeonly  set to true to only receive active plugins
   * \param response Reference to a std::string used to store the json response string
   * \return  0 when successful
   */
  int GetPluginServices(bool activeonly, Json::Value& response);

  /**
   * \brief AreRecordingSharesAccessible
   * \param thisplugin the plugin to check
   * \param response Reference to a std::string used to store the json response string
   * \return  0 when successful
   */
  int AreRecordingSharesAccessible(Json::Value& thisplugin, Json::Value& response);

  /**
   * \brief TuneLiveStream
   * \param channel_id  The ArgusTV ChannelID of the channel
   * \param stream      Reference to a string that will point to the tsbuffer file/RTSP stream
   */
  int TuneLiveStream(const std::string& channel_id,
                     ChannelType channeltype,
                     const std::string channelname,
                     std::string& stream);

  /**
   * \brief Stops the last tuned live stream
   */
  int StopLiveStream();

  /**
   * \brief Returns the URL of the current live stream
   */
  std::string GetLiveStreamURL(void);

  /**
   * \brief Returns the Signal information of the current live stream
   */
  int SignalQuality(Json::Value& response);

  /**
   * \brief Tell the recorder/tuner we are still showing this stream and to keep it alive. Call this every 30 seconds or so.
   */
  bool KeepLiveStreamAlive();

  /**
   * \brief Fetch the list of availalable channels for tv or radio
   * \param channeltype  The type of channel to fetch the list for
   */
  int GetChannelList(enum ChannelType channelType, Json::Value& response);

  /**
   * \brief Fetch the EPG data for the given guidechannel id
   * \param guidechannel_id  String containing the ARGUS guidechannel_id (not the channel_id)
   * \param epg_start        Start from this date
   * \param epg_stop         Until this date
   */
  int GetEPGData(const std::string& guidechannel_id,
                 struct tm epg_start,
                 struct tm epg_end,
                 Json::Value& response);

  /**
   * \brief Fetch the recording groups sorted by title
   * \param response Reference to a std::string used to store the json response string
   */
  int GetRecordingGroupByTitle(Json::Value& response);

  /**
   * \brief Fetch the detailed data for all recordings for a given title
   * \param title Program title of recording
   * \param response Reference to a std::string used to store the json response string
   */
  int GetFullRecordingsForTitle(const std::string& title, Json::Value& response);

  /**
   * \brief Fetch the detailed information of a recorded show
   * \param id unique id (guid) of the recording
   * \param response Reference to a std::string used to store the json response string
   */
  int GetRecordingById(const std::string& id, Json::Value& response);

  /**
   * \brief Mark this recording as watched
   * \param recordingpath
   */
  int SetRecordingLastWatched(const std::string& recordingfilename);

  /**
   * \brief Get the last watched position for this recording
   * \param recordingfilename full UNC path of the recording file
   * \param response Reference to a std::string used to store the json response string
   * \return last watched position in seconds, -1 on a failure
   */
  int GetRecordingLastWatchedPosition(const std::string& recordingfilename, Json::Value& response);

  /**
   * \brief Save the last watched position for this recording
   * \param recordingfilename full UNC path of the recording file
   * \param lastwatchedposition last watched position in seconds
   */
  int SetRecordingLastWatchedPosition(const std::string& recordingfilename,
                                      int lastwatchedposition);

  /**
   * \brief Set the play count for this recording
   * \param recordingfilename full UNC path of the recording file
   * \param playcount the number of times this recording was played
   */
  int SetRecordingFullyWatchedCount(const std::string& recordingfilename, int playcount);

  /**
   * \brief Delete the recording on the pvr backend
   * \param recordingfilename UNC filename to delete
   */
  int DeleteRecording(const std::string recordingfilename);

  /**
   * \brief Fetch the detailed information of a schedule
   * \param id unique id (guid) of the schedule
   * \param response Reference to a std::string used to store the json response string
   */
  int GetScheduleById(const std::string& id, Json::Value& response);

  /**
   * \brief Fetch the detailed information of a guide program
   * \param id unique id (guid) of the program
   * \param response Reference to a std::string used to store the json response string
   */
  int GetProgramById(const std::string& id, Json::Value& response);

  /**
   * \brief Fetch the list of schedules for tv or radio
   * \param channeltype  The type of channel to fetch the list for
   */
  int GetScheduleList(enum ChannelType channelType, Json::Value& response);

  /**
   * \brief Fetch the list of upcoming programs from type 'recording'
   * \currently not used
   */
  int GetUpcomingPrograms(Json::Value& response);

  /**
   * \brief Fetch the list of upcoming recordings
   */
  int GetUpcomingRecordings(Json::Value& response);

  /**
   * \brief Fetch the list of currently active recordings
   */
  int GetActiveRecordings(Json::Value& response);

  /**
   * \brief Cancel a currently active recording
   */
  int AbortActiveRecording(Json::Value& activeRecording);

  /**
   * \brief Cancel an upcoming program
   */
  int CancelUpcomingProgram(const std::string& scheduleid,
                            const std::string& channelid,
                            const time_t starttime,
                            const std::string& upcomingprogramid);

  /**
   * \brief Retrieve an empty schedule from the server
   */
  int GetEmptySchedule(Json::Value& response);

  /**
   * \brief Add a xbmc timer as a one time schedule
   */
  int AddOneTimeSchedule(const std::string& channelid,
                         const time_t starttime,
                         const std::string& title,
                         int prerecordseconds,
                         int postrecordseconds,
                         int lifetime,
                         Json::Value& response);

  /**
   * \brief Add a xbmc timer as a manual schedule
   */
  int AddManualSchedule(const std::string& channelid,
                        const time_t starttime,
                        const time_t duration,
                        const std::string& title,
                        int prerecordseconds,
                        int postrecordseconds,
                        int lifetime,
                        Json::Value& response);

  /**
   * \brief Delete a ArgusTV schedule
   */
  int DeleteSchedule(const std::string& scheduleid);

  /**
   * \brief Get the upcoming programs for a given schedule
   */
  int GetUpcomingProgramsForSchedule(const Json::Value& schedule, Json::Value& response);

  /**
   * \brief Get the upcoming recordings for a given schedule
   */
  int GetUpcomingRecordingsForSchedule(const std::string& scheduleid, Json::Value& response);

  /*
   * \brief Get the list with TV channel groups from ARGUS
   */
  int RequestTVChannelGroups(Json::Value& response);

  /*
   * \brief Get the list with Radio channel groups from ARGUS
   */
  int RequestRadioChannelGroups(Json::Value& response);

  /*
   * \brief Get the list with channels for the given channel group from ARGUS
   * \param channelGroupId GUID of the channel group
   */
  int RequestChannelGroupMembers(const std::string& channelGroupId, Json::Value& response);

  /*
   * \brief Get the logo for a channel
   * \param channelGUID GUID of the channel
   */
  std::string GetChannelLogo(const std::string& channelGUID);

  /*
   * \brief Subscribe to ARGUS TV service events
   */
  int SubscribeServiceEvents(int eventGroups, Json::Value& response);

  /*
   * \brief Unsubscribe from ARGUS TV service events
   */
  int UnsubscribeServiceEvents(const std::string& monitorId);

  /*
   * \brief Retrieve the ARGUS TV service events
   */
  int GetServiceEvents(const std::string& monitorId, Json::Value& response);

  /*
   * \brief Convert a XBMC Lifetime value to the ARGUS keepUntilMode setting
   * \param lifetime the XBMC lifetime value (in days)
   */
  int lifetimeToKeepUntilMode(int lifetime);

  /*
   * \brief Convert a XBMC Lifetime value to the ARGUS keepUntilValue setting
   * \param lifetime the XBMC lifetime value (in days)
   */
  int lifetimeToKeepUntilValue(int lifetime);

  static time_t WCFDateToTimeT(const std::string& wcfdate, int& offset);
  static std::string TimeTToWCFDate(const time_t thetime);

private:
  int RequestChannelGroups(enum ChannelType channelType, Json::Value& response);
  int GetLiveStreams();

  //Remember the last LiveStream object to be able to stop the stream again
  Json::Value m_currentLivestream;

  std::string m_baseURL;
  std::mutex m_communicationMutex;
}; // class ArgusTV
