<var name="backButtonTitle"><text>Back</text></var>
<var name="forwardButtonTitle"><text>Forward</text></var>
<var name="homeButtonTitle"><text>Home</text></var>
<var name="menuButtonTitle"><text>Menu</text></var>
<var name="pickerButtonTitle"><text>Picker</text></var>
<var name="glossaryButtonTitle"><text>Glossary</text></var>
<var name="generalButtonTitle"><text>General help articles</text></var>

<widget id="Help-dialog" type="GUI/Dialog">
    <setting name="title"><text>Help Desk</text></setting>
    <setting name="subTitle"></setting>
    <setting name="fullScreen">{generalButtonTitle}</setting>
    <setting name="minWidth">400</setting>
    <setting name="minHeight">500</setting>
    <setting name="width">400</setting>
    <setting name="height">500</setting>
    <setting name="content">
        <content>
            <div class="Help-controls">
                <a href="javascript:Help.back();" class="Help-control-button back" title="{backButtonTitle}"></a>
                <a href="javascript:Help.forward();" class="Help-control-button forward" title="{forwardButtonTitle}"></a>
                <a href="javascript:Help.home();" class="Help-control-button home" title="{homeButtonTitle}"></a>
                <a href="javascript:Help.toggleMenu();" class="Help-control-button menu" title="{menuButtonTitle}"></a>
                <a href="javascript:Help.picker();" class="Help-control-button picker" title="{pickerButtonTitle}"></a>
                <a href="javascript:Help.glossary();" class="Help-control-button glossary" title="{glossaryButtonTitle}"></a>
                <a href="javascript:Help.general();" class="Help-control-button general" title="{generalButtonTitle}"></a>
                <dataProvider action="Help::getSystemsMenu" />
            </div>
            <iframe frameborder="0" border="0" name="Help-iframe" id="Help-iframe" width="100%" height="100%" src=""></iframe>
        </content>
    </setting>
</widget>
