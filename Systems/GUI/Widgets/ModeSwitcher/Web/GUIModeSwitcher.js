function GUIModeSwitcher(id, settings)
{
    GUIContentSwitcher.call(this, id, settings);
    this.className = 'GUIModeSwitcher';

}

GUIModeSwitcher.prototype = {

}

dfx.noInclusionInherits('GUIModeSwitcher', 'GUIContentSwitcher', true);