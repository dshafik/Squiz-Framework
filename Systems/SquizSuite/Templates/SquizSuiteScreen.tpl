<div id="SquizSuiteScreen">
    <widget type="GUI/Box" id="squizSuite-currentProduct-box">
        <setting name="title"><text>Current Product</text></setting>
        <setting name="content">
            <widget type="GUI/Table" id="currProductTable">
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
    <widget type="GUI/Box" id="squizSuite-connectedProduct-box">
        <setting name="title"><text>Connected Products</text></setting>
        <setting name="headerContent">
            <content>
                <div class="addNewButtonContainer">
                    <widget type="GUI/Button" id="addNewProduct">
                        <setting name="value"><text>Add New</text></setting>
                        <setting name="click">SquizSuiteScreen.addProduct()</setting>
                    </widget>
                </div>
            </content>
        </setting>
        <setting name="content">
            <widget type="GUI/Table" id="connProductsTable">
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
                    <delIcon>
                        <name />
                        <align>left</align>
                    </delIcon>
                </setting>
                <setting name="allowDelete">false</setting>
                <setting name="rows">
                    <dataProvider action="SquizSuite::getConnectedProductRows" />
                </setting>
            </widget>
        </setting>
    </widget>
    <dataProvider action="SquizSuite::getSquizSuiteScreenInitCode">
        <arg></arg>
    </dataProvider>
</div>