<div class="Toolbar">
    <div class="logo"></div>
    <div class="leftList">
        <widget type="GUI/Toolbar/List" id="toolbarLeftList" />
    </div>
    <div class="rightList">
        <widget type="GUI/Toolbar/List" id="toolbarRightList" />
    </div>
    <div class="SaveButton">
        <widget type="GUI/Toolbar/SaveButton" id="toolbarSaveButton" />
    </div>

    <div class="button1">
        <widget type="GUI/Button" id="toolbarButton1">
            <setting name="name" value="Button ONE" />
            <setting name="onclick" value="this.clickedButton1();" />
        </widget>
    </div>
    <div class="button2">
        <widget type="GUI/Button" id="toolbarButton2" onclick="this.clickedButton2();" />
    </div>
    <div class="button3">
        <widget type="GUI/Button" id="toolbarButton3" />
    </div>
</div>
