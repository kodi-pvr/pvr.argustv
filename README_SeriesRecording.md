Series recording is implemented via two new GUI XML files (RecordPrefs.xml and DeteleTime.xml) that must be copied into your skin folder.   Currently only Confluence is supported.

If you use Confluence, the recommendation is to copy the confluence skin folder to your home addons folder under a different name.  This will ensure that nothing is overwritten during an update.

Steps:

1.  Find your skin.confluence folder.   On Linux this is usually /usr/local/share/kodi/addons.  On Windows it's your app install folder, e.g. C:\Program Files (x86)\Kodi\addons
2.  Copy skin.confluence and everything in it to your home userdata/addons folder.  See http://kodi.wiki/view/Userdata.
3.  Rename that folder from skin.confluence to e.g. skin.myconfluence, then open the addon.xml in your skin folder with an editor and edit the following lines:
	
id="skin.confluence"
to:
id="skin.myconfluence"

..and

name="Confluence"
to:
name="My Confluence"

4.  Install the pvr.argustv Addon.
5.  Copy the two XML files from $(KODI_HOME)/addons/pvr.argustv/resources/skins/skin.confluence/720p to the /720p folder in your skin folder you created above.
6.  Go back to KODI (restart probably needed) and choose your new skin.

Done.