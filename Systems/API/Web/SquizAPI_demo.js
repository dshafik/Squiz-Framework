sfapi.rootUrl = 'http://squiz-search.net';

function log(msg) {
    document.getElementById('target').innerHTML = document.getElementById('target').innerHTML + '<br />' + msg;
}

function clearlog() {
    document.getElementById('target').innerHTML = ' ';
}

function demoAPI(systemid, actionid, options) {
    clearlog();
    sfapi.get(systemid, actionid, options, function(data) {
        log(data.result);
    });
}