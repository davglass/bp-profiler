class BrowserProfiler
  # at the time the first function is invoked on the service, initialize
  # will be called.  Arguments will 
  def initialize(args)
    log("init called with #{args.pretty_inspect}")
  end
  
  def GetUsage(bp, args)
    x = `ps -ocomm,pcpu,rss -xwwc | grep firefox-bin | head -1`
    cpu = /\d+\.\d+/.match(x)[0]
    rss = /\d+$/.match(x)[0]
    bp.complete([Float(cpu), Integer(rss) * 1024])
  end
end

rubyCoreletDefinition = {
  'class' => "BrowserProfiler",
  'name' => "BrowserProfiler",
  'major_version' => 0,
  'minor_version' => 0,
  'micro_version' => 1,
  'documentation' => 'A service that analyzes the memory and cpu usage of a webbrowser.',
  'functions' =>
  [
    {
      'name' => 'GetUsage',
      'documentation' => "access an array containing the current cpu usage and resident memory.  Horrid - osx only - assumes ffx - broken for multiple instances.  very much proof of concept, people.",
      'arguments' =>
      [
      ]
    }  
  ] 
}
