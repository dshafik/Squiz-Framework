function GUITable(id) {
    var widgetURL  = GUI.getWidgetURL('/GUI/Table');
    var sortScript = widgetURL + '/jquery.tablesorter.min.js';
    dfx.includeScript(sortScript, function() {
        $("#"+id+"").tablesorter();
    });

}
