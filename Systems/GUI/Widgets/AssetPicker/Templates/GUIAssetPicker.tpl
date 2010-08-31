<widget id="AssetPicker" type="GUI/AssetPicker">
    <setting name="content">
          <widget id="AssetPicker-dialog" type="GUI/Dialog">
            <setting name="title"><text>Asset Finder</text></setting>
            <setting name="subTitle"><text>Select the asset you want to view, edit or use</text></setting>
            <setting name="fullScreen">true</setting>
            <setting name="content">
                <content>
                    <widget type="GUI/Lineage" id="AssetPicker-lineage"></widget>
                    <widget id="AssetPicker-assetBrowser" type="GUI/ColumnBrowser">
                        <setting name="enableMultiSelect">true</setting>
                        <setting name="lineageWidgetId">AssetPicker-lineage</setting>
                        <setting name="dynamicLoading">true</setting>
                        <setting name="dynamicDataProvider">
                            <system>GUIAssetPicker</system>
                            <method>getAssets</method>
                        </setting>
                        <setting name="items">
                            <dataProvider action="GUIAssetPicker::getAssets"></dataProvider>
                        </setting>
                    </widget>
                </content>
            </setting>
            <setting name="bottomContent">
                <content>
                    <widget id="AssetPicker-selectAsset" type="GUI/Button">
                        <setting name="value"><text>Select Asset(s)</text></setting>
                        <setting name="click">GUI.getWidget('AssetPicker').selectAssets()</setting>
                    </widget>
                    <widget id="AssetPicker-cancel" type="GUI/Button">
                        <setting name="value"><text>Cancel</text></setting>
                        <setting name="click">GUI.getWidget('AssetPicker').close()</setting>
                    </widget>
                </content>
            </setting>
          </widget>
      </setting>
</widget>
