package it.nerdphone
import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.provider.Settings
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import org.json.JSONObject

class MainActivity:ComponentActivity(){
 private val permission=registerForActivityResult(ActivityResultContracts.RequestPermission()) { }
 override fun onCreate(savedInstanceState:Bundle?){super.onCreate(savedInstanceState)
  if(Build.VERSION.SDK_INT>=33&&checkSelfPermission(Manifest.permission.POST_NOTIFICATIONS)!=PackageManager.PERMISSION_GRANTED)permission.launch(Manifest.permission.POST_NOTIFICATIONS)
  setContent { MaterialTheme(colorScheme=darkColorScheme(primary=Color(0xFFFFB347),background=Color(0xFF0B1017),surface=Color(0xFF151E29))) { Screen() } }
 }
 @Composable private fun Screen(){
  val prefs=remember {getSharedPreferences("config",MODE_PRIVATE)}
  var host by remember {mutableStateOf(prefs.getString("host","pool.nerdminers.org")!!)}
  var port by remember {mutableStateOf(prefs.getString("port","3333")!!)}
  var user by remember {mutableStateOf(prefs.getString("user","")!!)}
  var password by remember {mutableStateOf(prefs.getString("password","x")!!)}
  var stats by remember {mutableStateOf(JSONObject(Native.snapshot()))}
  var rate by remember {mutableDoubleStateOf(0.0)}
  var error by remember {mutableStateOf("")}
  var cpus by remember {mutableStateOf(JSONObject(Native.cpuInfo()))}
  val available=cpus.optInt("available",1).coerceAtLeast(1)
  var selectedThreads by remember {mutableIntStateOf(prefs.getInt("threads",1).coerceIn(1,available))}
  var testing by remember {mutableStateOf(false)}
  var testResult by remember {mutableStateOf("")}
  var workerRates by remember {mutableStateOf(listOf<Pair<Int,Double>>())}
  val scope=rememberCoroutineScope()

  LaunchedEffect(Unit){var last=stats.optLong("hashes");var time=android.os.SystemClock.elapsedRealtime();var lastWorkers=mapOf<Int,Long>();while(true){delay(1000);val s=JSONObject(Native.snapshot());val now=android.os.SystemClock.elapsedRealtime();val dt=(now-time).coerceAtLeast(1);val count=s.optLong("hashes");rate=if(s.optBoolean("running")&&count>=last)(count-last)*1000.0/dt else 0.0
   val arr=s.optJSONArray("workers");val next=mutableMapOf<Int,Long>();val rates=mutableListOf<Pair<Int,Double>>()
   if(arr!=null)for(i in 0 until arr.length()){val w=arr.getJSONObject(i);val id=w.optInt("id");val h=w.optLong("hashes");val old=lastWorkers[id];next[id]=h;rates.add(id to if(s.optBoolean("running")&&old!=null&&h>=old)(h-old)*1000.0/dt else 0.0)}
   workerRates=rates;lastWorkers=next;last=count;time=now;stats=s}}
  val active=stats.optBoolean("running")
  Column(Modifier.fillMaxSize().background(MaterialTheme.colorScheme.background).verticalScroll(rememberScrollState()).padding(24.dp),verticalArrangement=Arrangement.spacedBy(14.dp)) {
   Spacer(Modifier.height(16.dp))
   Text("NERDPHONE",color=MaterialTheme.colorScheme.primary,fontSize=14.sp,fontWeight=FontWeight.Bold)
   Text("Bitcoin. One hash at a time.",fontSize=28.sp,fontWeight=FontWeight.Bold,color=Color.White)
   Text("SHA-256d / SOLO / MULTI-THREADED CPU MINER",fontSize=11.sp,color=Color.Gray)
   Card(Modifier.fillMaxWidth()){Column(Modifier.padding(20.dp)){
    Text("HASH RATE",color=Color.Gray,fontSize=12.sp)
    Text("%.2f kH/s".format(java.util.Locale.US, rate/1000),fontSize=38.sp,fontWeight=FontWeight.Bold,color=MaterialTheme.colorScheme.primary)
    Text(stats.optString("status"),color=Color.White)
   }}
   Card(Modifier.fillMaxWidth()){Column(Modifier.padding(16.dp),verticalArrangement=Arrangement.spacedBy(8.dp)){
    Text("CPU DETECTION & MINING THREADS",color=MaterialTheme.colorScheme.primary,fontWeight=FontWeight.Bold)
    Text("Available to app: $available · Online: ${cpus.optInt("online")} · Configured: ${cpus.optInt("configured")}",color=Color.White,fontSize=13.sp)
    Text("Allowed logical CPU IDs: ${cpus.optJSONArray("allowedCpuIds") ?: "unavailable"}",color=Color.Gray,fontSize=11.sp)
    Text("Selected miners: $selectedThreads + 1 dedicated pool thread",color=Color.White,fontSize=13.sp)
    Row(horizontalArrangement=Arrangement.spacedBy(8.dp)){
     OutlinedButton(onClick={selectedThreads--},enabled=!active&&!testing&&selectedThreads>1){Text("−")}
     OutlinedButton(onClick={selectedThreads++},enabled=!active&&!testing&&selectedThreads<available){Text("+")}
     OutlinedButton(onClick={selectedThreads=available},enabled=!active&&!testing){Text("Use all")}
    }
    OutlinedButton(onClick={cpus=JSONObject(Native.cpuInfo());selectedThreads=selectedThreads.coerceIn(1,cpus.optInt("available",1).coerceAtLeast(1));testResult=""},enabled=!active&&!testing){Text("Detect CPUs again")}
    Button(onClick={scope.launch {
     testing=true;error=""
     try{val output=withContext(Dispatchers.Default){Native.benchmark(selectedThreads)};val b=JSONObject(output)
      val lines=mutableListOf("${b.optInt("threads")} miners · ${"%.2f".format(java.util.Locale.US,b.optDouble("seconds"))} s · ${"%.2f".format(java.util.Locale.US,b.optDouble("hashrate")/1000)} kH/s total")
      val ws=b.getJSONArray("workers");for(i in 0 until ws.length()){val w=ws.getJSONObject(i);lines.add("Miner ${w.optInt("id")}: ${"%.2f".format(java.util.Locale.US,w.optDouble("hashrate")/1000)} kH/s")}
      testResult=lines.joinToString("\n")
     }catch(e:Exception){error=e.message ?: "CPU test failed"}finally{testing=false}
    }},enabled=!active&&!testing){Text(if(testing)"Testing CPUs…" else "Run 3-second offline test")}
    if(testResult.isNotBlank())Text(testResult,color=Color.White,fontSize=12.sp)
    if(active){Text("Active miners: ${stats.optInt("miningThreads",1)}",color=Color.White,fontSize=13.sp);workerRates.forEach{(id,h)->Text("Miner $id · ${"%.2f".format(java.util.Locale.US,h/1000)} kH/s",color=Color.Gray,fontSize=12.sp)}}
    Text("CPU detection runs on this device. Threads are scheduled by Android, not pinned to physical cores. The short offline test submits no shares and does not predict sustained thermal performance. Keep this screen open during the test.",color=Color.Gray,fontSize=11.sp)
   }}
   Row(horizontalArrangement=Arrangement.spacedBy(10.dp)){Stat("SUBMITTED",stats.optLong("sent").toString(),Modifier.weight(1f));Stat("ACCEPTED",stats.optLong("accepted").toString(),Modifier.weight(1f))}
   Row(horizontalArrangement=Arrangement.spacedBy(10.dp)){Stat("REJECTED",stats.optLong("rejected").toString(),Modifier.weight(1f));Stat("SHARE DIFF","%.8g".format(java.util.Locale.US, stats.optDouble("difficulty",0.0)),Modifier.weight(1f))}
   Card(Modifier.fillMaxWidth()){Column(Modifier.padding(16.dp),verticalArrangement=Arrangement.spacedBy(6.dp)){
    Text("STRATUM JOBS / DIAGNOSTICS",color=MaterialTheme.colorScheme.primary,fontWeight=FontWeight.Bold)
    Text("Received: ${stats.optLong("jobsReceived")} · Assigned: ${stats.optLong("jobsAssigned")} · Started: ${stats.optLong("jobsStarted")}",color=Color.White,fontSize=13.sp)
    Text("Assigned job: ${stats.optString("assignedJob").ifBlank { "—" }}",color=Color.White,fontSize=12.sp)
    Text("Mining job: ${stats.optString("workingJob").ifBlank { "none / switching jobs" }}",color=Color.White,fontSize=12.sp)
    Text("Hashes on current job: ${stats.optLong("jobHashes")} · Job diff: ${stats.optDouble("workingDifficulty",0.0)}",color=Color.White,fontSize=12.sp)
    val jobAge=stats.optDouble("jobAge",-1.0)
    val rxAge=stats.optDouble("rxAge",-1.0)
    Text("Last job: ${if(jobAge<0) "never" else "%.0f s ago".format(java.util.Locale.US, jobAge)} · clean_jobs: ${stats.optBoolean("cleanJobs")}",color=Color.Gray,fontSize=12.sp)
    Text("Last pool message: ${stats.optString("lastMessage").ifBlank { "—" }} · ${if(rxAge<0) "never" else "%.0f s ago".format(java.util.Locale.US, rxAge)}",color=Color.Gray,fontSize=12.sp)
    Text("Jobs confirm protocol activity; only an accepted share verifies the end-to-end submission path.",color=Color.Gray,fontSize=11.sp)
   }}
   Text("Requested diff: 0.001 · Pool-assigned diff: ${stats.optDouble("difficulty",0.0)}",color=Color.Gray,fontSize=12.sp)
   Text("User agent: ${stats.optString("userAgent", "NerdMinerV2/V1.8.3")}",color=Color.Gray,fontSize=12.sp)
   Text("Difficulty requests sent: ${stats.optLong("suggestionRequests")}",color=Color.Gray,fontSize=12.sp)
   Text(stats.optString("suggestionReply", "Not sent"),color=Color.Gray,fontSize=12.sp)
   if(active && stats.optDouble("difficulty",0.0)>0.001) Text("The pool has not assigned 0.001. Mining uses the pool target, not the requested value.",color=MaterialTheme.colorScheme.primary,fontSize=12.sp)
   Text("Network difficulty: ${"%.8g".format(java.util.Locale.US, stats.optDouble("networkDifficulty",0.0))}",color=Color.Gray,fontSize=12.sp)
   Text("Unknown outcome: ${stats.optLong("unknown")} · Queue drops: ${stats.optLong("dropped")}",color=Color.Gray,fontSize=12.sp)
   val candidate=stats.optString("lastBlock").ifBlank {getSharedPreferences("events",MODE_PRIVATE).getString("candidate","").orEmpty()}
   if(candidate.isNotBlank()) Card {Column(Modifier.padding(16.dp)){Text("⚠ BLOCK CANDIDATE",color=MaterialTheme.colorScheme.primary);Text(candidate,color=Color.White);Text("Network target met locally. Pool confirmation still needs verification.",color=Color.Gray)}}
   Text("STRATUM V1 CONNECTION",color=Color.Gray,fontSize=12.sp)
   OutlinedTextField(host,{host=it},label={Text("Pool host")},singleLine=true,enabled=!active,modifier=Modifier.fillMaxWidth())
   OutlinedTextField(port,{port=it},label={Text("TCP port")},singleLine=true,enabled=!active,modifier=Modifier.fillMaxWidth())
   OutlinedTextField(user,{user=it},label={Text("BTC address / worker")},singleLine=true,enabled=!active,modifier=Modifier.fillMaxWidth())
   OutlinedTextField(password,{password=it},label={Text("Pool password (never a seed or private key)")},singleLine=true,enabled=!active,modifier=Modifier.fillMaxWidth())
   if(error.isNotBlank())Text(error,color=MaterialTheme.colorScheme.error)
   Button(onClick={
    if(active)startService(Intent(this@MainActivity,MiningService::class.java).setAction("STOP"))
    else if(host.isBlank()||host.contains("://")||(port.toIntOrNull() ?: 0) !in 1..65535||user.isBlank())error="Enter a host without a URL scheme, a valid port and your BTC address / worker."
    else {error="";prefs.edit().putString("host",host.trim()).putString("port",port).putString("user",user.trim()).putString("password",password).putInt("threads",selectedThreads).apply();startForegroundService(Intent(this@MainActivity,MiningService::class.java))}
   },enabled=!testing,modifier=Modifier.fillMaxWidth().height(54.dp)){Text(if(active)"STOP MINING" else "START MINING")}
   OutlinedButton(onClick={startActivity(Intent(Settings.ACTION_IGNORE_BATTERY_OPTIMIZATION_SETTINGS))},modifier=Modifier.fillMaxWidth()){Text("Android battery settings")}
   Text("No app-level CPU cap. More miners increase heat and battery use. Mining can heat your phone and drain its battery. Android may throttle or stop the service. Finding a block is extremely unlikely.",color=Color.Gray,fontSize=12.sp)
   if(stats.optString("error").isNotBlank())Text("Last event: ${stats.optString("error")}",color=Color.Gray,fontSize=12.sp)
   Text("Protocol reference: NerdMiner V1.8.3 · Experimental build",color=Color.Gray,fontSize=11.sp)
  }
 }
 @Composable private fun Stat(label:String,value:String,modifier:Modifier){Card(modifier){Column(Modifier.padding(16.dp)){Text(label,color=Color.Gray,fontSize=10.sp);Text(value,color=Color.White,fontSize=22.sp,fontWeight=FontWeight.Bold)}}}
}
