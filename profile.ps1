if (Test-Path "./ndkpath.txt")
{
    $NDKPath = Get-Content ./ndkpath.txt
} else {
    $NDKPath = $ENV:ANDROID_NDK_HOME
}

python $NDKPath/simpleperf/app_profiler.py -p com.beatgames.beatsaber -a com.unity3d.player.UnityPlayerActivity -lib .\build\debug\ --ndk_path $NDKPath --record_options=--exit-with-parent -r "--call-graph fp"

if ($?) {
    python $NDKPath/simpleperf/report_html.py
}