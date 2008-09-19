/*
 * Signals and Slots for Prototype: Easy custom javascript events
 * http://tetlaw.id.au/view/blog/signals-and-slots-for-prototype-easy-custom-javascript-events
 * Andrew Tetlaw
 * Version 1.2 (2006-06-19)
 *
 * http://creativecommons.org/licenses/by-sa/2.5/
 */
Signal = {
	throwErrors : true,
	MT : function(){ return true },
	connect : function(obj1, func1, obj2, func2, options) {
		var options = Object.extend({
			connectOnce : false,
			before : false,
			mutate : function() {return arguments;}
		}, options || {});
		if(typeof func1 != 'string' || typeof func2 != 'string') return;

		var sigObj = obj1 || window;
		var slotObj = obj2 || window;
		var signame = func1+'__signal_';
		var slotsname = func1+'__slots_';
		if(!sigObj[signame]) {
			// having the slotFunc in a var and setting it by using an anonymous function in this way
			// is apparently a good way to prevent memory leaks in IE if the objects are DOM nodes.
			var slotFunc = function() {
				var args = [];
				for(var x = 0; x < arguments.length; x++){
					args.push(arguments[x]);
				}
				args = options.mutate.apply(null,args)
				var result;
				if(!options.before) result = sigObj[signame].apply(sigObj,arguments); //default: call sign before slot
				sigObj[slotsname].each(function(slot){
					try {
						if(slot && slot[0]) { // testing for null, a disconnect may have nulled this slot
							slot[0][slot[1]].apply(slot[0],args); //[0] = obj, [1] = func name
						}
					} catch(e) {
						if(Signal.throwErrors) throw e;
					}
				});
				if(options.before) result = sigObj[signame].apply(sigObj,arguments); //call slot before sig
				return result; //return sig result
			};
			(function() {
				sigObj[slotsname] = $A([]);
				sigObj[signame] = sigObj[func1] || Signal.MT;
				sigObj[func1] = slotFunc;
			})();
		}
		var con = (sigObj[slotsname].length > 0) ? 
					(options.connectOnce ? !sigObj[slotsname].any(function(slot) { return (slot[0] == slotObj && slot[1] == func2) }) : true) : 
					true;
		if(con) {
			sigObj[slotsname].push([slotObj,func2]);
		}
	},
	connectOnce : function(obj1, func1, obj2, func2, options) {
		Signal.connect(obj1, func1, obj2, func2, Object.extend(options || {}, {connectOnce : true}))
	},
	disconnect : function(obj1, func1, obj2, func2, options) {
		var options = Object.extend({
			disconnectAll : false
		}, options || {});
		if(typeof func1 != 'string' || typeof func2 != 'string') return;

		var sigObj = obj1 || window;
		var slotObj = obj2 || window;
		var signame = func1+'__signal_';
		var slotsname = func1+'__slots_';

		// I null them in this way so that any currectly active signal will read a null slot,
		// otherwise the slot will be applied even though it's been disconnected
		if(sigObj[slotsname]) {
			if(options.disconnectAll) {
				sigObj[slotsname] = sigObj[slotsname].collect(function(slot) {
					if(slot[0] == slotObj && slot[1] == func2) {
						slot[0] = null;
						return null;
					} else {
						return slot;
					}
				}).compact();
			} else {
				var idx = -1;
				sigObj[slotsname] = sigObj[slotsname].collect(function(slot, index) {
					if(slot[0] == slotObj && slot[1] == func2 && idx < 0) {  //disconnect first match
						idx = index;
						slot[0] = null;
						return null;
					} else {
						return slot;
					}
				}).compact();
			}
		}
	},
	disconnectAll : function(obj1, func1, obj2, func2, options) {
		Signal.disconnect(obj1, func1, obj2, func2, Object.extend(options || {}, {disconnectAll : true}))
	}
}