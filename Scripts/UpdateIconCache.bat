echo Killing Explorer Process to Update Icon Cache...
call taskkill /IM explorer.exe /F
echo Deleting Icon cache...
call CD /d %userprofile%\AppData\Local
call DEL IconCache.db /a
echo Restarting Explorer...
call explorer.exe
