function GUISelect(id, settings)
{
    this.id       = id;
    this.settings = settings;

}

GUISelect.prototype = {

    getValue: function()
    {
        return dfx.getId(this.id).selectedIndex;

    }

};
