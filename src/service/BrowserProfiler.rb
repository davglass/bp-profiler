class BrowserProfiler
  def initialize(args)
    @takingSamples = true
  end
  
  def start(bp, args)

    if (args['interval'] < 0.1)
      args['interval'] = 0.1
    end
    
    @takingSamples = true
    Thread.new(bp, args['callback'], args['interval']) do |bp,callback,interval |
      while @takingSamples
        x = `ps -ocomm,pcpu,rss -xwwc | grep firefox-bin | head -1`
        cpu = /\d+\.\d+/.match(x)[0]
        rss = /\d+$/.match(x)[0]
        callback.invoke({'cpu' => Float(cpu), 'memory' => Integer(rss) * 1024})
        sleep interval
      end
      bp.complete()
    end
  end
  
  def stop(bp, args)
    @takingSamples = false
    bp.complete()
  end

end

rubyCoreletDefinition = {
  'class' => "BrowserProfiler",
  'name' => "BrowserProfiler",
  'major_version' => 0,
  'minor_version' => 0,
  'micro_version' => 2,
  'documentation' => 'A service that analyzes the memory and cpu usage of a webbrowser.',
  'functions' =>
  [
    {
      'name' => 'start',
      'documentation' => "Takes samples of memory/cpu every 'interval' seconds.  Calls supplied callback with map of {memory, cpu}. Horrid - osx only - assumes ffx - broken for multiple instances.  very much proof of concept, people.",
      'arguments' =>
      [
         {
            'name' => 'interval',
            'type' => 'double',
            'required' => true,
            'documentation' => 'The sample time in seconds.'
          },
          {
            'name' => 'callback',
            'type' => 'callback',
            'required' => true,
            'documentation' => 'the callback to send a hello message to'
          }
      ]
    },

    {
      'name' => 'stop',
      'documentation' => "Stop taking samples.",
      'arguments' =>[]
    }  
  ] 
}