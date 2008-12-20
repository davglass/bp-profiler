class TimingTest
  # Constructor
  def initialize(args)
  end

  def youGotTime(bp, args)
    args['startedCallback'].invoke(nil)
    
    startTime = Time.now    

    bp.complete(samps)
  end
end

rubyCoreletDefinition = {
  'class' => "TimingTest",
  'name'  => "TimingTest",
  'major_version' => 0,
  'minor_version' => 0,
  'micro_version' => 1,
  'documentation' => 
    'A service that returns some timing samples to help us discover how to better correlate time between the browser and an external browserplus service.',
  'functions' =>
  [
    {
      'name' => 'youGotTime',
      'documentation' => "immediately calls a callback.  Then samples for 10 seconds attaining both datestamps and relative ms granularity timing.  With some magic in JS we can compare the relative accuracy of the two methods.",
      'arguments' =>
      [
          {
            'name' => 'startedCallback',
            'type' => 'callback',
            'required' => true,
            'documentation' => 'a callback that will be hit immediately to allow for calibration to get a valid shared "zero point" between processes'
          }
      ]
    }
  ] 
}
