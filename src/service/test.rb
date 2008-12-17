
require 'BrowserProfiler.rb'

def pr(val)
  str = "stamp=#{val['stamp']}, time=#{val['time']}"
  return str
end

def printer(val)
  if (val.class == Hash)
    puts "Single sample returned: " + pr(val)
  elsif (val.class == Array)
    puts "Samples returned: #{val.length}"
    val.each do |v|
      puts "  " + pr(v)
    end
  else
    puts "What was that?"
  end
end

class BPProxy
  def complete(val)
    puts "COMPLETE:"
    printer(val)
  end
end

class Invoker
  def invoke(val)
    puts "CALLBACK:"
    printer(val)
  end
end

bp = BPProxy.new
profiler = BrowserProfiler.new([])


if false
  profiler.sample(bp, [])
else
  invoker = Invoker.new
  profiler.start(bp, {'callback'=>invoker, 'interval'=>0.5})
  sleep 3
  profiler.stop(bp, [])
  sleep 1
end

