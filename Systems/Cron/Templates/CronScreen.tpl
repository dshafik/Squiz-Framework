<div id="CronScreen">
    <div class="CronSettingsBoxContainer GUI-screen-left">
        <widget type="GUI/TextBox" id="test" />
    </div>
    <div class="CronScreenMainContent GUI-screen-right">
        <widget type="GUI/Box" id="CronScreenMainBox">
            <setting name="title" value="Cron Jobs" />
            <setting name="content">
                <widget type="GUI/Table" id="cronJobsList">
                    <setting name="showBorders" value="true" />
                    <setting name="selectable" value="false" />
                    <setting name="columns">
                        <job>
                            <align>left</align>
                            <name>Jobs</name>
                        </job>
                        <lastRun>
                            <align>left</align>
                            <name>Last Run</name>
                        </lastRun>
                        <frequency>
                            <align>center</align>
                            <name>Frequency</name>
                        </frequency>
                        <enabled>
                            <align>center</align>
                            <name>Enabled</name>
                        </enabled>
                    </setting>
                    <setting name="dataProvider" value="Cron::getCronScreenJobsList" />
                </widget>
            </setting>
        </widget>
    </div>
</div>
