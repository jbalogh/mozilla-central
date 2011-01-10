function test() {
  waitForExplicitFinish();

  loadAndWait("data:text/plain,1", function () {
    loadAndWait("data:text/plain,2", function () {
      loadAndWait("data:text/plain,3", runTests);
    });
  });
}

function runTests() {
  duplicate(0, "maintained the original index", function () {
    gBrowser.removeCurrentTab();

    duplicate(-1, "went back", function () {
      duplicate(1, "went forward", function () {
        gBrowser.removeCurrentTab();
        gBrowser.removeCurrentTab();
        finish();
      });
    });
  });
}

function duplicate(delta, msg, cb) {
  var start = gBrowser.sessionHistory.index;

  duplicateTabIn(gBrowser.selectedTab, "tab", delta);

  gBrowser.selectedBrowser.addEventListener("pageshow", function () {
    gBrowser.selectedBrowser.removeEventListener("pageshow", arguments.callee, false);
    is(gBrowser.sessionHistory.index, start + delta, msg);
    executeSoon(cb);
  }, false);
}

function loadAndWait(url, cb) {
  gBrowser.selectedBrowser.addEventListener("load", function () {
    gBrowser.selectedBrowser.removeEventListener("load", arguments.callee, true);
    executeSoon(cb);
  }, true);

  gBrowser.loadURI(url);
}
