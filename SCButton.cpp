#include "SCButton.h"

SCButton::SCButton(void* instance, String name, bool isInverted, uint16_t delayUntilFirstReactionIn10MS, uint16_t delayAfterFirstReactionIn10MS, dlgOnButton onButton, dlgGetButtonState getButtonState) {
  mPeriod = new SCPeriod(this, 10, true, SCButton::OnPeriodExpiredStatic);
  //--------------------------------------
  mCurrentButtonState = false;
  mLastButtonState = false;
  mTimeUntilNextReaction = 0;
  //--------------------------------------
  mInstance = instance;
  mOnButton = onButton;
  mGetButtonState = getButtonState;
  //--------------------------------------
  mName = name;
  mIsInverted = isInverted;
  mDelayUntilFirstReactionIn10MS = delayUntilFirstReactionIn10MS;
  mDelayAfterFirstReactionIn10MS = delayAfterFirstReactionIn10MS;
}
SCButton::~SCButton() {
}
void SCButton::Check() {
  mPeriod->Check();
}
void SCButton::OnPeriodExpiredStatic(void* instance) {
  SCButton* bt = (SCButton*)instance;
  if (bt != NULL) bt->OnPeriodExpired();
}
void SCButton::OnPeriodExpired() {
  if (!mGetButtonState) return;
  mCurrentButtonState = mGetButtonState(mName) ^ mIsInverted;
  if (mLastButtonState != mCurrentButtonState)  //Button state changed.
  {
    if (mCurrentButtonState)  //Just now Button Pressed.
    {
      if (mOnButton) mOnButton(mInstance, mName, true, true);
      mTimeUntilNextReaction = mDelayUntilFirstReactionIn10MS;
    } 
    else  //Just now Button Released.
    {
      if (mOnButton) mOnButton(mInstance, mName, false, true);
      mTimeUntilNextReaction = 0;
    }
    mLastButtonState = mCurrentButtonState;
  } 
  else  //Button state unchanged.
  {
    if (mCurrentButtonState)  //Button is Pressed.
    {
      if (mTimeUntilNextReaction > 0)  //Wait until next reaction.
      {
        if (mTimeUntilNextReaction == 1)  //Perform next reaction.
        {
          if (mOnButton) mOnButton(mInstance, mName, true, false);
          mTimeUntilNextReaction = mDelayAfterFirstReactionIn10MS;
        }
        mTimeUntilNextReaction--;
      }
    }
  }
}