function GUIToggleButton(id, settings) {
    this.id = id;
    this.settings = settings;

    var widgetElement = dfx.getId(id);
    var selectedElement = dfx.getClass('selected', widgetElement)[0];
    var currentClass = selectedElement.className.split(' ');

    this.currValue = (dfx.inArray('yes', currentClass) === true) ? true : false;

    GUI.addWidgetEvent(this, 'toggleOn');
    GUI.addWidgetEvent(this, 'toggleOff');
    
    this.init();
}

GUIToggleButton.prototype = {
    init: function() {

        var self = this;
        dfx.addEvent(dfx.getId(self.id), 'click', function() {

                if (self.currentValue === true) {
                    // move to false
                    self.setValue(false);
                } else {
                    // move to true
                    self.setValue(true);
                }
        });

    }, 

    getValue: function() {
        var self = this;
        return self.currentValue;
    },

    setValue: function(value) {
        var self = this;
        if (value !== self.currentValue) {
            // Only do something if the value is different from the current value
            self.currentValue = value;
            if (value === true) {
                // move to true
                self.switchOn();
            } else {
                // move to false
                self.switchOff();
            }
        }
    },

    switchOff: function() {
        var sec = 300; 
        var self     = this;
        var widgetElement = dfx.getId(self.id);
        var glowMask = dfx.getClass('toggle', widgetElement)[0];
        dfx.animate(glowMask, {left: 50}, sec, function() {

            var optYes = dfx.getClass('yes', widgetElement)[0];
            dfx.removeClass(optYes, 'selected');
            var optNo = dfx.getClass('no', widgetElement)[0];
            dfx.addClass(optNo, 'selected');

            // finish, fire event
            self.fireToggleOffCallbacks();
        });
    },

    switchOn: function() {
        var sec = 300; 
        var self     = this;
        var widgetElement = dfx.getId(self.id);
        var glowMask = dfx.getClass('toggle', widgetElement)[0];
        dfx.animate(glowMask, {left: 6}, sec, function() {

            var optYes = dfx.getClass('yes', widgetElement)[0];
            dfx.addClass(optYes, 'selected');
            var optNo = dfx.getClass('no', widgetElement)[0];
            dfx.removeClass(optNo, 'selected');

            // finish, fire event
            self.fireToggleOnCallbacks();
        });
    }
}
