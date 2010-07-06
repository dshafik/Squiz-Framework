function GUITextBox(id, settings) {
    this.id = id;
	this.settings = settings;
	var widgetElement = dfx.getId(self.id);
	var textBox = dfx.getClass('input', widgetElement)[0];

	this.init();
}

GUITextBox.prototype = {
	init: function() {

		var self = this;
		var widgetElement = dfx.getId(self.id);
		var textBox = dfx.getClass('input', widgetElement)[0];

		dfx.addEvent(textBox, 'blur', function() {
            dfx.removeClass(textBox, 'selected');
		});

		dfx.addEvent(textBox, 'focus', function() {
            dfx.addClass(textBox, 'selected');
		});

    }, 

	getValue: function() {
		var self = this;
		var widgetElement = dfx.getId(self.id);
		var textBox = dfx.getClass('input', widgetElement)[0];
		return textBox.value;
	},

	setValue: function(value) {
		var self = this;
		var widgetElement = dfx.getId(self.id);
		var textBox = dfx.getClass('input', widgetElement)[0];
		if (value !== textBox.value) {
			// Only do something if the value is different from the current value
			textBox.value = value;
		}
	}
}
