(function(){
    
    'use strict';
    
    // library object
    var aa = {};
    
    // basic animation node
    function Animation(){
        this.action = null; // action to take
        this.startTime = null;
        this.endTime = null;
        this.children = [];
        this.isLoop = false;
    }
    
    var fmod = function (a,b) { return Number((a - (Math.floor(a / b) * b)).toPrecision(8)); };
    
    // play an anmation
    Animation.prototype.play = function(parentTime){
        
        var localTime;
        var runAction = false;
        
        // compute local time
        if (this.isLoop){
            localTime = fmod(parentTime - this.startTime, this.endTime - this.startTime);
            runAction = true;
        } else if (this.startTime != null){
            localTime = parentTime - this.startTime;
            if (this.endTime != null){
                if (parentTime >= this.startTime && parentTime < this.endTime){
                    runAction = true;
                }
            } else {
                if (parentTime >= this.startTime){
                    runAction = true;
                }
            }
        } else {
            // no start time
            if (this.endTime != null){
                // but we have end time
                localTime = parentTime - endTime;
                if (parentTime < this.endTime){
                    runAction = true;
                }
            } else {
                // not even end time, well passthrough
                localTime = parentTime;
                runAction = true;
            }
        }
        
        // perform action if set
        if (runAction && this.action != null){
            this.action(localTime);
        }
        
        // call play on children
        for (var i=0; i<this.children.length; i++){
            this.children[i].play(localTime);
        }
    }

    // builder
    aa.anim = function(startTime, endTime, fn){
        // create new node
        var node = new Animation();
        // populate it
        for (var i=0; i<arguments.length; i++){
            var argument = arguments[i];
            switch (typeof argument){
                case "number":
                    if (node.startTime == null){
                        node.startTime = argument;
                    } else if (node.endTime == null){
                        node.endTime = argument;
                    } else {
                        console.error("bad argument ",i,argument);
                    }
                    break;
                case "function":
                    if (node.action == null){
                        node.action = argument;
                    } else {
                        console.error("bad argument ",i,argument);
                    }
                    break;
                case "object":
                    if (Animation.prototype.isPrototypeOf(argument)){
                        node.children.push(argument);
                    } else {
                        console.log("bad argument", argument, typeof argument);
                    }
                    break;
                default:
                    console.log("bad argument", argument, typeof argument);
                    break;
            }
        }
        return node;
    }
    
    //create a loop
    aa.loop = function(){
        var node = aa.anim.apply(this, arguments);
        node.isLoop = true;
        if (node.startTime == null && node.endTime == null){
            node.startTime = 0;
            node.endTime = 1;
        } else if (node.startTime != null && node.endTime == null){
            node.endTime = node.startTime;
            node.startTime = 0;
        } else if (node.startTime == null && node.endTime != null){
            node.startTime = 0;
        }
        return node;
    }
    
    var schedulerId = null;
    var scheduledItems = [];
    function schedulerProcess(timeMs){
        var time = timeMs / 1000.0;
        // run all scheduled items
        for (var i=0; i<scheduledItems.length; i++){
            var item = scheduledItems[i];
            item.play(time);
        }
        
        // todo: stop when animations end instead of endlessly polling
        schedulerId = requestAnimationFrame(schedulerProcess);
    }
    
    
    aa.play = function(object){
        var startScheduler = false;
        // check object
        if (Animation.prototype.isPrototypeOf(object)){
            // todo: calculate end time of anim
            scheduledItems.push(aa.anim(performance.now()/1000.0, object));
            startScheduler = true;
        } else {
            console.error('unexpected argument',object);
        }
        // start scheduler if necesarry
        schedulerId = requestAnimationFrame(schedulerProcess);
    }

    // export module
    if (typeof define !== "undefined" && typeof require !== "undefined"){
        // AMD module
        define([], function(){
            return aa;
        })
    } else if (typeof module !== "undefined" && typeof module.exports !== "undefined"){
        // CommonJS
        module.exports = aa;
    } else if (typeof window != "undefined") {
        // frontend
        window.AnimateAnything = aa;
    } else {
        console.error("failed to export library");
    }

})();