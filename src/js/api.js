(function() {

    BPProfiler = function() {
    };

    var proto = {
        startMemory: function() {
            //Call BP to start monitoring memory
        },
        stopMemory: function() {
            //Call BP to stop monitoring memory
            var memData = {};
            return memData;
        },
        startCPU: function() {
            //Call BP to start monitoring cpu usage
        },
        stopCPU: function() {
            //Call BP to stop monitoring cpu usage
            var cpuData = {};
            return cpuData;
        },
        start: function() {
            //Start page profiling..
            this.startMemory();
            this.startCPU();
        },
        stop: function() {
            //Stop page profiling..
            this.mem = this.stopMemory();
            this.cpu = this.stopCPU();
            //Return the data from BP..
            return {
                memory: this.mem,
                cpu: this.cpu
            };
        },
        report: function() {
            //Return detailed data from BP..
            return {
                memory: this.mem,
                cpu: this.cpu
            };
        }
    };

    BPProfiler.prototype = proto;

})();
