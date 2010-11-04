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
    <setting name="position">fixed</setting>
    <setting name="posRight">25px</setting>
    <setting name="posTop">70px</setting>
    <setting name="persistent">true</setting>
    <setting name="content">
        <content>
            <div class="Help-controls">
                <a href="javascript:void(0);" onclick="Help.back();" class="Help-control-button back" title="{backButtonTitle}"></a>
                <a href="javascript:void(0);" onclick="Help.forward();" class="Help-control-button forward" title="{forwardButtonTitle}"></a>
                <a href="javascript:void(0);" onclick="Help.home();" class="Help-control-button home" title="{homeButtonTitle}"></a>
                <a href="javascript:void(0);" onclick="Help.toggleMenu();" class="Help-control-button menu" title="{menuButtonTitle}"></a>
                <a href="javascript:void(0);" onclick="Help.picker();" class="Help-control-button picker" title="{pickerButtonTitle}"></a>
                <a href="javascript:void(0);" onclick="Help.glossary();" class="Help-control-button glossary" title="{glossaryButtonTitle}"></a>
                <a href="javascript:void(0);" onclick="Help.general();" class="Help-control-button general" title="{generalButtonTitle}"></a>
                <widget id="Help-search" type="GUI/TextBox">
                    <setting name="size">20</setting>
                    <setting name="requiresSave">false</setting>
                    <setting name="hint"><text>Search...</text></setting>
                </widget>
                <dataProvider action="Help::getSystemsMenu" />
            </div>
            <div id="Help-iframeWrapper" class="Help-iframeWrapper">
                <div id="Help-dialog-msg-pointer" class="Help-dialog-msg">
                    <dataProvider action="Help::printMessageBox">
                        <arg><text>Click on a section of the screen to view more information about it.</text></arg>
                        <arg><text>Cancel</text></arg>
                        <arg>info</arg>
                    </dataProvider>
                </div>
                <div id="Help-dialog-msg-pointer-noInfo" class="Help-dialog-msg">
                    <dataProvider action="Help::printMessageBox">
                        <arg><text>There is no help information for the part of the screen that you have
                        clicked on. Please click on another part of the screen or click Cancel.</text></arg>
                        <arg><text>Cancel</text></arg>
                        <arg>info</arg>
                    </dataProvider>
                </div>
                <div id="Help-dialog-msg-screenChanged" class="Help-dialog-msg">
                    <dataProvider action="Help::printMessageBox">
                        <arg><text>You have switched screens. Click on the Home button to view the Suggestions for the current screen.</text></arg>
                        <arg><text>Hide this message</text></arg>
                        <arg>info</arg>
                        <arg>true</arg>
                    </dataProvider>
                </div>
                <div id="Help-dialog-msg-welcomeMsg" class="Help-dialog-msg">
                    <var name="productType"><dataProvider action="SquizSuite::getProductType" /></var>
                    <dataProvider action="Help::printMessageBox">
                        <arg>
                            <text>
                                <value>Welcome to %s. Please take the time to read these general help articles and familiarise yourself with the system.</value>
                                <args>
                                    <arg>{productType}</arg>
                                </args>
                            </text>
                        </arg>
                        <arg><text>Hide this message</text></arg>
                        <arg>info</arg>
                        <arg>true</arg>
                    </dataProvider>
                </div>
                <div class="Help-overlay" id="Help-overlay"></div>
                <iframe name="Help-iframe" id="Help-iframe" frameborder="0" border="0" width="100%" height="100%" src=""></iframe>
            </div>
        </content>
    </setting>
</widget>
