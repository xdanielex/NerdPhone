package it.nerdphone

import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.BatteryManager
import android.os.Build
import android.os.PowerManager
import java.io.File

// Temperature sampling lives outside the UI on purpose: mining normally runs with the
// screen off, and a peak recorded only while the activity is visible would be the
// screen-on peak, which is not the interesting one.
object Thermal {
 @Volatile var battery:Double=Double.NaN; private set
 @Volatile var zone:Double=Double.NaN; private set
 @Volatile var peak:Double=Double.NaN; private set
 @Volatile var status:Int=-1; private set
 @Volatile var samples:Int=0; private set

 fun reset(){ peak=Double.NaN; samples=0 }

 fun sample(ctx:Context,count:Boolean=true){
  try{
   val i=ctx.registerReceiver(null,IntentFilter(Intent.ACTION_BATTERY_CHANGED))
   val raw=i?.getIntExtra(BatteryManager.EXTRA_TEMPERATURE,-1) ?: -1
   if(raw>0) battery=raw/10.0
   if(Build.VERSION.SDK_INT>=29) status=ctx.getSystemService(PowerManager::class.java).currentThermalStatus
   zone=readMaxThermalZone()
   val hottest=when{
    battery.isNaN() -> zone
    zone.isNaN() -> battery
    else -> maxOf(battery,zone)
   }
   if(!hottest.isNaN()&&(peak.isNaN()||hottest>peak)) peak=hottest
   if(count) samples+=1
  }catch(t:Throwable){}
 }

 // Returns the hottest readable SoC-ish thermal zone in °C, or NaN. On Android 10+
 // SELinux usually blocks /sys/class/thermal, so this is best effort only.
 private fun readMaxThermalZone():Double{
  var max=Double.NaN
  try{
   val zones=File("/sys/class/thermal").listFiles() ?: return Double.NaN
   for(f in zones){
    if(!f.name.startsWith("thermal_zone"))continue
    try{
     val type=File(f,"type").readText().trim().lowercase()
     if(!(type.contains("cpu")||type.contains("soc")||type.contains("tsens")||type.contains("ap")))continue
     val raw=File(f,"temp").readText().trim().toDouble()
     val c=if(raw>1000) raw/1000.0 else raw
     if(c>0&&c<120&&(max.isNaN()||c>max))max=c
    }catch(t:Throwable){}
   }
  }catch(t:Throwable){}
  return max
 }
}
