<div id="SquizSuiteScreen">
    <widget type="GUI/Box" id="squizSuite-currentProduct-box">
        <setting name="title"><text>Current Product</text></setting>
        <setting name="content">
            <widget type="GUI/Table" id="squizSuiteList">
                <setting name="columns">
                    <currTypeIcon>
                        <name><text>Type</text></name>
                        <align>left</align>
                    </currTypeIcon>
                    <currName>
                        <name><text>Name</text></name>
                        <align>left</align>
                    </currName>
                    <currStatusIcon>
                        <name><text>Status</text></name>
                        <align>left</align>
                    </currStatusIcon>
                    <currSummary>
                        <name>Summary</name>
                        <align>left</align>
                    </currSummary>
                </setting>
                <setting name="hideHeader">true</setting>
                <setting name="rows">
                    <dataProvider action="SquizSuite::getCurrentProductRow" />
                </setting>
            </widget>
        </setting>
    </widget>
</div>
