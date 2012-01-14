let Cu = Components.utils;
let Ci = Components.interfaces;
let Cc = Components.classes;

Cu.import("resource://gre/modules/XPCOMUtils.jsm");
Cu.import("resource://gre/modules/Services.jsm");

function Push() {}
Push.prototype = {

  classID: Components.ID("{a49c18f8-3379-4fc0-8c90-d7772c1a9ff3}"),

  QueryInterface: XPCOMUtils.generateQI([Ci.nsIDOMGlobalPropertyInitializer]),

  // nsIDOMGlobalPropertyInitializer
  init: function (aWindow) {
    this._window = aWindow;

    let self = this;

    let chromeObject = {

      requestPermission: function(cb) {
        let pushEvent = {
            host: self._window.host,
            ts: Date.now(),
        };
        pushEvent.wrappedJSObject = pushEvent;
        Services.obs.notifyObservers(pushEvent, "request-push", 5);

        cb("callback");
      },

      __exposedProps__: {
        requestPermission: "r"
      }
    };

    // We need to return an actual content object here, instead of a wrapped
    // chrome object. This allows things like console.log.bind() to work.
    let contentObj = Cu.createObjectIn(aWindow);
    function genPropDesc(fun) {
      return { enumerable: true, configurable: true, writable: true,
               value: chromeObject[fun].bind(chromeObject) };
    }
    const properties = {
      requestPermission: genPropDesc("requestPermission"),
      __mozillaConsole__: { value: true }
    };

    Object.defineProperties(contentObj, properties);
    Cu.makeObjectPropsNormal(contentObj);

    return contentObj;
  },
};

let NSGetFactory = XPCOMUtils.generateNSGetFactory([Push]);
