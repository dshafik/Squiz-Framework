<div id="SuperUsersScreen">
    <widget type="GUI/Box" id="superUsers-box">
        <setting name="title"><text>Super Users</text></setting>
        <setting name="headerContent">
            <content>
                <div class="addNewButtonContainer">
                    <widget type="GUI/Button" id="addNewSuperUser">
                        <setting name="value"><text>Add Existing User</text></setting>
                    </widget>
                </div>
            </content>
        </setting>
        <setting name="content">
            <widget type="GUI/Table" id="superUsersList">
                <setting name="columns">
                    <user>
                        <name><text>User</text></name>
                        <align>left</align>
                    </user>
                    <email>
                        <name><text>Email</text></name>
                        <align>left</align>
                    </email>
                    <lastLogin>
                        <name><text>Last Login</text></name>
                        <align>left</align>
                    </lastLogin>
                    <remove>
                        <name> </name>
                        <align>left</align>
                    </remove>
                </setting>
                <setting name="rows">
                    <dataProvider action="User::getSuperUsersList" />
                </setting>
            </widget>
        </setting>
    </widget>
    <div style="margin-top: 20px; border:1px solid #222;">
        <widget type="GUI/Lineage" id="assetTypesLineage">
            <setting name="lineage">
                <!-- Get the lineage from ColumnBrowser widget
                <dataProvider action="GUIColumnBrowser::getLineage">
                    <arg><widget-settings id="assetTypes" /></arg>
                </dataProvider> -->
                <value>
                    <id>asset</id>
                    <title><text>Asset</text></title>
                </value>
                <value>
                    <id>page</id>
                    <title><text>Page</text></title>
                </value>
                <value>
                    <id>assetListing</id>
                    <title><text>Asset Listing Page</text></title>
                </value>
            </setting>
        </widget>
    </div>

    <div style="height: 200px; margin-top: 20px; border:1px solid #222;">
        <widget type="GUI/ColumnBrowser" id="assetTypes">
            <setting name="enableMultiSelect">true</setting>
            <setting name="lineageWidgetId">assetTypesLineage</setting>
            <setting name="items">
                <value>
                    <id>asset</id>
                    <itemContent><text>Asset</text></itemContent>
                    <children>
                        <value>
                            <id>page</id>
                            <itemContent><text>Page</text></itemContent>
                             <children>
                                <value>
                                    <id>accountManager</id>
                                    <itemContent><text>Account Manager</text></itemContent>
                                </value>
                                <value>
                                    <id>assetListing</id>
                                    <itemContent><text>Asset Listing</text></itemContent>
                                </value>
                            </children>
                        </value>
                        <value>
                            <id>file</id>
                            <itemContent><text>File</text></itemContent>
                            <children>
                                <value>
                                    <id>image</id>
                                    <itemContent><text>Image</text></itemContent>
                                </value>
                                <value>
                                    <id>cssDocument</id>
                                    <itemContent><text>CSS</text></itemContent>
                                </value>
                                <value>
                                    <id>wordDocument</id>
                                    <itemContent><text>Word Document</text></itemContent>
                                </value>
                                <value>
                                    <id>pdfDocument</id>
                                    <itemContent><text>PDF Document</text></itemContent>
                                </value>
                                <value>
                                    <id>movie</id>
                                    <itemContent><text>Movie</text></itemContent>
                                </value>
                                <value>
                                    <id>gif</id>
                                    <itemContent><text>GIF Animation</text></itemContent>
                                </value>
                                <value>
                                    <id>orsmDoc</id>
                                    <itemContent><text>ORSM DOC</text></itemContent>
                                </value>
                            </children>
                        </value>
                        <value>
                            <id>folder</id>
                            <itemContent><text>Folder</text></itemContent>
                        </value>
                        <value>
                            <id>design</id>
                            <itemContent><text>Design</text></itemContent>
                        </value>
                        <value>
                            <id>user</id>
                            <itemContent><text>User</text></itemContent>
                        </value>
                    </children>
                </value>
            </setting>
        </widget>
    </div>
</div>
