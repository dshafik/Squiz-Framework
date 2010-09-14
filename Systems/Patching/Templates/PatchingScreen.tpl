<div id="PatchingScreen">
    <div class="PatchingSettingsBoxContainer GUI-screen-left">
        <!-- Start Cront Screen settings box -->
        <widget type="GUI/ScreenSettings" id="patchingSettings">
            <setting name="title"><text>System Information</text></setting>
            <setting name="subTitle">
                <content>
                    <text>
                        <value>Current Version: %s</value>
                        <args>
                            <arg><dataProvider action="Patching::getCurrentRevision" /></arg>
                        </args>
                    </text>
                </content>
            </setting>
            <setting name="sections">
                <dataProvider action="Patching::getPatchingScreenSettings" />
            </setting>
        </widget>
        <!-- End Cront Screen settings box -->
    </div>
    <div class="PatchingScreenMainContent GUI-screen-right">
        <!-- Start Cront Screen Jobs List -->
        <widget type="GUI/Box" id="PatchingScreenMainBox">
            <setting name="title"><text>Cron Jobs</text></setting>
            <setting name="content">
                <div>hello world</div>
            </setting>
        </widget>
        <!-- End Cront Screen Jobs List -->
    </div>
</div>
