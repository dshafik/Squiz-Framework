<widget id="ProjectSwitcher-dialog" type="GUI/Dialog">
    <setting name="title"><text>Projects</text></setting>
    <setting name="subTitle"><text>Click the project you would like to view</text></setting>
    <setting name="fullScreen"><text>Project Switcher</text></setting>
    <setting name="minWidth">600</setting>
    <setting name="minHeight">575</setting>
    <setting name="width">600</setting>
    <setting name="height">575</setting>
    <setting name="position">fixed</setting>
    <setting name="content">
        <content>
            <div class="ProjectSwitcher-content">
                <dataProvider action="Project::getProjectSwitcherContent" />
            </div>
        </content>
    </setting>
</widget>
