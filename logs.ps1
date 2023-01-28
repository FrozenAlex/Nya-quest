param (
    [string]$p1
 )

if (Test-Path ./logs/) {
    Write-Host "Folder Exists"
}else
{
    new-item ./logs/ -itemtype directory  | Out-Null
}


adb pull "/storage/emulated/0/Android/data/com.beatgames.beatsaber/files/logs/paper/Nya.log" ./logs/
adb pull "/storage/emulated/0/ModData/com.beatgames.beatsaber/Configs/Nya.json" ./logs/