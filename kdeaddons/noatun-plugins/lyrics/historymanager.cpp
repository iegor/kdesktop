#include "historymanager.h"

/** HISTORY MANAGER ***
*** TODO: Add more complex support (like in Konqueror)
          Maybe use Konqueror's History Manager */

HistoryManager::HistoryManager(QObject *parent) : QObject(parent) {
}

HistoryManager::~HistoryManager() {
}

void HistoryManager::addURL(const KURL &url) {
  /* push current URL to the back */
  if (!currentURL.isEmpty()) {
    if (back_stack.count() == 0)
      emit uiChanged( Back, true );
    back_stack.push(currentURL);
  }
  /* Make a copy of the new URL */
  currentURL = url;
  /* Clear the forward stack */
  if (forward_stack.count() > 0)
    emit uiChanged( Forward, false );
  forward_stack.clear();
}

KURL HistoryManager::back() {
  if (back_stack.count() <= 0)
    return KURL();
  /* The currentURL is now pushed in the forward_stack */
  if (forward_stack.count() == 0)
    emit uiChanged( Forward, true );
  forward_stack.push(currentURL);
  /* The last URL is the back stack is now popped as the current one */
  if (back_stack.count() == 1)
    emit uiChanged( Back, false );
  currentURL = back_stack.pop();
  return currentURL;
}

KURL HistoryManager::forward() {
  if (forward_stack.count() <= 0)
    return KURL();
  /* Push the currentURL into the back_stack */
  if (back_stack.count() == 0)
    emit uiChanged( Back, true );
  back_stack.push(currentURL);
  /* Pop from the forward_stack into the currentURL */
  if (forward_stack.count() == 1)
    emit uiChanged( Forward, false );
  currentURL = forward_stack.pop();
  return currentURL;
}

#include "historymanager.moc"
