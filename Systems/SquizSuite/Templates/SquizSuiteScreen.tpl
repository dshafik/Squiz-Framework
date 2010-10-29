<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<div id="SquizSuiteScreen" class="GUI-screen-middle">
    <widget type="GUI/Box" id="squizSuite-currentProduct-box">
        <setting name="title"><text>Current Product</text></setting>
        <setting name="content">
            <widget type="GUI/Table" id="currProductTable">
                <setting name="columns">
                    <currTypeIcon>
                        <name><text>Type</text></name>
                        <align>left</align>
                        <width>60px</width>
                    </currTypeIcon>
                    <currName>
                        <name><text>Name</text></name>
                        <align>left</align>
                        <width>420px</width>
                    </currName>
                    <currStatusIcon>
                        <name><text>Status</text></name>
                        <align>middle</align>
                        <width>47px</width>
                    </currStatusIcon>
                    <currSummary>
                        <name>Summary</name>
                        <align>left</align>
                    </currSummary>
                </setting>
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
                        <setting name="click">SquizSuiteSquizSuiteScreen.addProduct()</setting>
                    </widget>
                </div>
            </content>
        </setting>
        <setting name="content">
            <widget type="GUI/Table" id="connProductsTable">
                <setting name="columns">
                    <productTypeIcon>
                        <name><text>Type</text></name>
                        <align>left</align>
                        <width>60px</width>
                    </productTypeIcon>
                    <productName>
                        <name><text>Name</text></name>
                        <align>left</align>
                        <width>420px</width>
                    </productName>
                    <productStatusIcon>
                        <name><text>Status</text></name>
                        <align>middle</align>
                        <width>47px</width>
                    </productStatusIcon>
                    <productSummary>
                        <name>Summary</name>
                        <align>left</align>
                    </productSummary>
                    <delIcon>
                        <name />
                        <align>left</align>
                        <width>25px</width>
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