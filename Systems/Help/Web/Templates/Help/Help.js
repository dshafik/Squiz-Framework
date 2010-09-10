var Help = new function()
{
    var _apiURL = null;

    this.init = function() {
        // Set the API URL.
        _apiURL = '/__api/raw/Help/getPage?_api_token=' + dfx.getId('__api_token').value + '&';

        var elem = dfx.getId('Help-dialog');

        var navHeight = parseInt(dfx.getElementHeight(dfx.getClass('Help-controls', elem)[0]));
        var iframe    = dfx.getId('Help-iframe');
        var self      = this;
        iframe.onload = function(e) {
            self.iframeLoaded();
        }

        var midElem    = dfx.getClass('GUIDialog-middle', elem)[0];
        var initHeight = parseInt(dfx.getElementHeight(midElem));

        dfx.setStyle(iframe, 'height', (initHeight - navHeight - 27) + 'px');

        GUI.getWidget('Help-dialog').addDialogResizedCallback(function(ui, contentSize) {
            dfx.setStyle(iframe, 'height', (contentSize.height - navHeight - 27) + 'px');
        });

    }

    this.refresh = function() {
        this.loadGlossaryPage();
    };

    this.loadGlossaryPage = function(template) {
        template = template || GUI.getCurrentTemplate();

        // Construct the glossary page id for this template.
        var pageid = template.split(':').join('-') + '-glossary-index.html';
        this.loadPage(pageid);

    };

    this.loadIndexPage = function(template) {
        template   = template || GUI.getCurrentTemplate();
        var pageid = template.split(':').join('-') + '-index.html';
    pageid = 'User-index.html';
        this.loadPage(pageid);
    };

    this.loadGeneralPage = function(template) {

    };

    this.loadPage = function(pageid) {
        var helpIframe = dfx.getId('Help-iframe');

        dfx.attr(helpIframe, 'src', _apiURL + 'pageid=' + escape(pageid));

    };

    this.iframeLoaded = function() {
        jQuery(dfx.getId('Help-iframe')).localScroll({offset: {top: -32}});
    };

    this.back = function() {
        history.back(1);
    };

    this.forward = function() {
        history.forward(1);
    };

    this.home = function() {
        this.loadIndexPage();
    };

    this.glossary = function() {
        this.loadGlossaryPage();
    };

    this.general = function() {
        this.loadGeneralPage();
    };


};
