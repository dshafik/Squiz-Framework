function GUISaveButton(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.init();

}

GUISaveButton.prototype = {

    init: function()
    {
        dfx.addEvent(dfx.getId(this.id), 'click', function() {
            GUI.save();
        });

    }

};
