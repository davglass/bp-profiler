#
# Profile the current browser (currently hacked )
# 
class BrowserProfiler

  # Reasonable number of samples, so we don't use too much memory
  @@MAX_SAMPLES = 1000

  # Constructor
  def initialize(args)
    @takingSamples = false
    @sampleList = []
    @maxSamples = 0
    @sampleNumber = 0
    @browsers = {'firefox' => 'firefox', 'safari' => 'Safari', 'ie' => 'iexplore'}
    @os = 'win' #default to windows
    if RUBY_PLATFORM.downcase.include?("darwin")
        @os = 'mac'
        @browsers = {'firefox' => 'firefox-bin', 'safari' => 'safari'}
    end
    
    #What comes in here??
  end

  #
  # Start the sampling process.  At most 1,000 samples are taken.  If no callback
  # is provided, the samples are stored and return after this method completes
  # or 1,000 samples are taken.
  #
  def start(bp, args)
    if @takingSamples == true
      bp.error("alreadyStarted", "you tried to start another sampling process, but we're already sampling")
      return
    end

    interval = args['interval'] || 1.0
    if (interval < 0.1)
      interval = 0.1
    end

    @startTime = Time.now
    @takingSamples = true
    @maxSamples = @@MAX_SAMPLES
    @sampleList = []
    @sampleNumber = 0
    Thread.new(bp, args['callback'], interval) do | bp,callback,interval |
      while @takingSamples
        @sampleNumber += 1
        s = _get_sample()
        if (callback)
          callback.invoke(s)
        else
          @sampleList.push(s)
        end

        @maxSamples -= 1
        if @maxSamples > 0
          sleep interval
        else
          break
        end

      end
      bp.complete(@sampleList)
    end
  end

  #
  # Take 1 sample.
  #
  def sample(bp, args)
    bp.complete(_get_sample())
  end

  #
  # Stop the sampling.  Return the sampleList if no callback was provided to start.
  #
  def stop(bp, args)
    @takingSamples = false
    bp.complete(@sampleList)
  end

    def _ps_browsers()
        res = []
        @browsers.each do |name, pat|
            res.push(_ps_browser(name, pat))
        end
        return res
    end

  #
  # Get sample data for the given browser.
  #
  def _ps_browser(name, pat)
    cpu = -1.0
    mem = -1.0
    
    if @os == 'mac'
        x = `ps -ocomm,pcpu,rss -xwwc | grep -i #{pat}`
        if x.length > 0
          cpu = Float(/\d+\.\d+/.match(x)[0])
          mem = Float(/\d+$/.match(x)[0])*1024.0
        end
    else
        #THIS IS REALLY HACKY !!!!!!!!
        #AND REALLY REALLY REALLY SLOW.. 2 seconds per sample :(
        #Damn Windows....
        cmdLine = "typeperf.exe \"\Process(#{pat})\\% Processor Time\" -sc 1"

        pipe = IO.popen(cmdLine)
        cmd = pipe.readlines
        pipe.close

        x1 = cmd[2]
        x1 = x1.split(%r{,\s*})


        cmdLine2 = "tasklist /fi \"IMAGENAME eq #{pat}.exe\" /fo CSV /nh"

        pipe = IO.popen(cmdLine2)
        cmd2 = pipe.readlines
        pipe.close

        x2 = cmd2[1]
        x2 = x2.split(%r{,\s*})
        
        
        if x1[1] and x2[4]
            #This needs some formatting!!
            cpu = x1[1]
            mem = x2[4]
        end
    end
    

    ret = { 'name' => name, 'cpu' => cpu, 'mem' => mem }
    return ret
  end
  
  #
  # Get a single sample.  Three system calls are made, 1 to iostat and 2 to ps
  #
  def _get_sample
    time = (Time.now - @startTime).to_f
    
    res = _ps_browsers()

    # Not sure how to get a full date stamp in ruby
    # But this get's me really close..
    # Get the time and convert it to a string
    stamp = "%10.3f" % Time.now.to_f
    # Not remove the . from the float after turning it into a string
    stamp = stamp.sub(/\./, '')

    return {'sample' => @sampleNumber, 'stamp' => stamp, 'time' => time, 'browsers' => res }

  end
end

rubyCoreletDefinition = {
  'class' => "BrowserProfiler",
  'name'  => "BrowserProfiler",
  'major_version' => 0,
  'minor_version' => 0,
  'micro_version' => 7,
  'documentation' => 
    'A service that analyzes the memory and cpu usage of a web browser.  ' +
    'The service can take 1 sample or multiple samples at a specified interval.  ' +
    'When sampling at intervals, at most 1,000 samples are taken.  If you provide ' + 
    'a callback function, your javascript will be called after every sample is taken.  ' + 
    'If no callback is provided, all samples are stored in an array and returned after start() ' + 
    'completes or stop() is called.\n' + 
    'The sample object is a map with the following keys (most values are floats):\n' +
    '[sample] - the sample number (1-1,000)\n' + 
    '[time]   - the offset time in seconds of when the sample was taken\n' + 
    '[sys]    - the percentage CPU "sys" processes are using\n' + 
    '[user]   - the percentage CPU "user" processes are using\n' + 
    '[ffxcpu] - the percentage CPU Firefox is using, or -1.0 if it is not running\n' +
    '[ffxmem] - the amout of memory Firefox is using, or -1.0 if it is not running\n' +
    '[safcpu] - the percentage CPU Safari is using, or -1.0 if it is not running\n' +
    '[safmem] - the amout of memory Safari is using, or -1.0 if it is not running\n',

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
            'required' => false,
            'documentation' => 'the callback to send a hello message to'
          }
      ]
    },

    {
      'name' => 'stop',
      'documentation' => "Stop taking samples.  Passes back the array of all samples.",
      'arguments' => []
    },
    
    {
      'name' => 'sample',
      'documentation' => "Returns a single sample, map of {memory, cpu}",
      'arguments' => []
    }
  ] 
}
