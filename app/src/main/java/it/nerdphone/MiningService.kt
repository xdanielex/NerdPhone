package it.nerdphone
import android.app.*
import android.content.Intent
import android.os.*
import org.json.JSONObject

class MiningService : Service() {
 private val handler = Handler(Looper.getMainLooper())
 private var wake: PowerManager.WakeLock? = null
 private var seenBlocks = 0L
 private var previousHashes = 0L
 private var previousTime = 0L
 private lateinit var manager: NotificationManager
 private val tick = object : Runnable {
  override fun run() {
   val s = JSONObject(Native.snapshot())
   val now = SystemClock.elapsedRealtime()
   val hashes = s.optLong("hashes")
   val rate = if(previousTime > 0) (hashes-previousHashes)*1000.0/(now-previousTime).coerceAtLeast(1) else 0.0
   previousTime=now; previousHashes=hashes
   val text = "${s.optString("status")} · ${"%.1f".format(java.util.Locale.US, rate/1000)} kH/s · ${s.optLong("accepted")} accepted"
   manager.notify(1, notification(text))
   val blocks = s.optLong("blocks")
   if(blocks>seenBlocks) {
    seenBlocks=blocks
    getSharedPreferences("events", MODE_PRIVATE).edit().putString("candidate", s.optString("lastBlock")).apply()
    val alert=Notification.Builder(this@MiningService,"blocks")
     .setSmallIcon(android.R.drawable.stat_notify_error)
     .setContentTitle("Bitcoin block candidate!")
     .setContentText("Network target met. Check confirmation with your pool.")
     .setStyle(Notification.BigTextStyle().bigText("Hash: ${s.optString("lastBlock")}\nThis is not yet a confirmed block."))
     .setContentIntent(openApp()).setAutoCancel(true).build()
    manager.notify(2,alert)
   }
   handler.postDelayed(this,1000)
  }
 }
 private fun openApp()=PendingIntent.getActivity(this,0,Intent(this,MainActivity::class.java),PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT)
 private fun notification(text:String):Notification {
  val stop=PendingIntent.getService(this,1,Intent(this,MiningService::class.java).setAction("STOP"),PendingIntent.FLAG_IMMUTABLE)
  return Notification.Builder(this,"mining").setSmallIcon(android.R.drawable.stat_notify_sync)
   .setContentTitle("NerdPhone · mining active").setContentText(text).setContentIntent(openApp())
   .setOngoing(true).addAction(Notification.Action.Builder(null,"Stop",stop).build()).build()
 }
 override fun onCreate(){
  super.onCreate()
  manager=getSystemService(NotificationManager::class.java)
  manager.createNotificationChannel(NotificationChannel("mining","Mining in progress",NotificationManager.IMPORTANCE_LOW))
  manager.createNotificationChannel(NotificationChannel("blocks","Block candidate",NotificationManager.IMPORTANCE_HIGH).apply { enableVibration(true) })
 }
 override fun onStartCommand(intent:Intent?,flags:Int,startId:Int):Int {
  if(intent?.action=="STOP"){stopSelf();return START_NOT_STICKY}
  startForeground(1,notification("Starting…"))
  if(JSONObject(Native.snapshot()).optBoolean("running"))return START_NOT_STICKY
  val p=getSharedPreferences("config",MODE_PRIVATE)
  val host=p.getString("host","").orEmpty();val user=p.getString("user","").orEmpty()
  if(host.isBlank()||user.isBlank()){stopSelf();return START_NOT_STICKY}
  wake=(getSystemService(POWER_SERVICE) as PowerManager).newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,"NerdPhone:Mining").apply { acquire() }
  try {
   Native.start(host,p.getString("port","3333")!!,user,p.getString("password","x")!!,p.getInt("threads",1))
   seenBlocks=0;previousHashes=0;previousTime=0
   handler.removeCallbacks(tick);handler.post(tick)
  } catch(e:Exception){stopSelf()}
  return START_NOT_STICKY
 }
 override fun onDestroy(){handler.removeCallbacks(tick);Native.stop();wake?.let {if(it.isHeld)it.release()};wake=null;stopForeground(STOP_FOREGROUND_REMOVE);super.onDestroy()}
 override fun onBind(intent:Intent?) : IBinder? = null
}
