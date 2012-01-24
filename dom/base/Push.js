let Cu = Components.utils;
let Ci = Components.interfaces;
let Cc = Components.classes;

Cu.import("resource://gre/modules/XPCOMUtils.jsm");
Cu.import("resource://gre/modules/Services.jsm");

let db = {};
let root = "http://push.jbalogh.me";

function log(s) {
  Services.console.logStringMessage(s);
}

function Push() {}
Push.prototype = {

  classID: Components.ID("{a49c18f8-3379-4fc0-8c90-d7772c1a9ff3}"),

  QueryInterface: XPCOMUtils.generateQI([Ci.nsIDOMGlobalPropertyInitializer]),

  // nsIDOMGlobalPropertyInitializer
  init: function (aWindow) {
    this._window = aWindow;

    let self = this;

    self.getToken(function(t) {
      log("we have a token: " + t);
      self.getQueue(t, "push.jbalogh.me", function(q) {
        log("token and queue: " + t + " : " + q);
      });
    });

    let chromeObject = {

      requestPermission: function(cb) {
        let pushEvent = {
            host: self._window.host,
            ts: Date.now(),
        };
        pushEvent.wrappedJSObject = pushEvent;
        Services.obs.notifyObservers(pushEvent, "request-push", 5);

        self.getToken(function(t) {
          log("token: " + t);
          self.getQueue(t, self._window.host, function(q) {
            log("token: " + t + " ;queue: " + q);
            cb(q);
          });
        });
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

  getToken: function(cb) {
    log("getting token");
    if (db.token) {
      log("have a token: " + db.token);
      return cb(db.token);
    }
    log("xhr token");

    let xhr = Cc["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance(Ci.nsIXMLHttpRequest);
    xhr.open("POST", root + "/token/", true);
    xhr.addEventListener("load",  (function() {
      if (xhr.status == 200) {
        db.token = JSON.parse(xhr.responseText).token;
        log("got token: " + db.token);
        return cb(db.token);
      } else {
        log("xhr token failed: " + xhr.status);
      }
    }).bind(this), false);

    xhr.addEventListener("error", function() { log("xhr error"); });
    xhr.send(null);
  },

  getQueue: function(token, host, cb) {
    log("getting a queue for: " + host);
    if (db.host) {
      log("existing queue: " + host + " => " + db.host);
      return cb(host);
    }

    log("xhr queue");
    let xhr = Cc["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance(Ci.nsIXMLHttpRequest);
    xhr.open("POST", root + "/queue/", true);
    xhr.addEventListener("load", function() {
      if (xhr.status == 200) {
        let response = JSON.parse(xhr.responseText);
        db.host = response.queue;
        log("new queue: " + host + " => " + db.host);
        return cb(db.host);
      } else {
        log("xhr queue failed: " + xhr.status);
      }
    });
    xhr.addEventListener("error", function() { log("xhr error"); });

    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.send("domain=" + host + "&token=" + token);
  },
};

let NSGetFactory = XPCOMUtils.generateNSGetFactory([Push]);
