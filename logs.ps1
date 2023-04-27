param (
    [string]$p1
 )

if (Test-Path ./logs/) {
    Write-Host "Folder Exists"
}else
{
    new-item ./logs/ -itemtype directory  | Out-Null
}


adb pull "/storage/emulated/0/Android/data/com.beatgames.beatsaber/files/logs" ./logs/

adb pull "/storage/emulated/0/Android/data/com.beatgames.beatsaber/files/tombstone_00" ./logs/
adb pull "/storage/emulated/0/Android/data/com.beatgames.beatsaber/files/tombstone_01" ./logs/
adb pull "/storage/emulated/0/Android/data/com.beatgames.beatsaber/files/tombstone_02" ./logs/


adb pull "/storage/emulated/0/ModData/com.beatgames.beatsaber/Configs/Nya.json" ./logs/