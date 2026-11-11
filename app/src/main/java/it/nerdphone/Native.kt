package it.nerdphone
object Native {
 init { System.loadLibrary("nerdphone") }
 external fun start(host: String, port: String, user: String, password: String, threads: Int)
 external fun cpuInfo(): String
 external fun benchmark(threads: Int): String
 external fun stop()
 external fun snapshot(): String
}
