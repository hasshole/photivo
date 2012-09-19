/*******************************************************************************
**
** Photivo
**
** Copyright (C) 2012 Bernd Schoeler <brjohn@brother-john.net>
**
** This file is part of Photivo.
**
** Photivo is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License version 3
** as published by the Free Software Foundation.
**
** Photivo is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Photivo.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include "ptFilter_Tone.h"
#include "ui_ptFilter_Tone.h"
#include "ptCfgItem.h"
#include "../ptImage.h"

//==============================================================================

const QString CToneId = "Tone";

const QString CSaturationAll    = "SaturationAll";
const QString CStrengthAll      = "StrengthAll";
const QString CHueAll           = "HueAll";

const QString CSaturationShad   = "SaturationShad";
const QString CStrengthShad     = "StrengthShad";
const QString CHueShad          = "HueShad";

const QString CSaturationMid    = "SaturationMid";
const QString CStrengthMid      = "StrengthMid";
const QString CHueMid           = "HueMid";

const QString CSaturationLights = "SaturationLights";
const QString CStrengthLights   = "StrengthLights";
const QString CHueLights        = "HueLights";

//==============================================================================

ptFilter_Tone::ptFilter_Tone()
: ptFilterBase()
{
//  FHelpUri = "http://photivo.org/something";
  internalInit();
}

//==============================================================================

ptFilterBase *ptFilter_Tone::createTone() {
  auto hInstance         = new ptFilter_Tone;
  hInstance->FFilterName = CToneId;
  hInstance->FCaption    = tr("Tone");
  return hInstance;
}

//==============================================================================

void ptFilter_Tone::doDefineControls() {
  FCfgItems = QList<ptCfgItem>()
    //            Id                       Type                      Default     Min           Max           Step        Decimals, commonConnect, storeable, caption, tooltip
    << ptCfgItem({CSaturationAll,          ptCfgItem::Slider,        1.0,        0.0,          4.0,          0.1,        2,        true, true, tr("Saturation"), tr("")})
    << ptCfgItem({CStrengthAll,            ptCfgItem::Slider,        0.0,        0.0,          1.0,          0.05,       2,        true, true, tr("Strength"), tr("")})
    << ptCfgItem({CHueAll,                 ptCfgItem::HueSlider,     60,         0,            360,          10,         0,        true, true, tr("Hue"), tr("")})
    << ptCfgItem({CSaturationShad,         ptCfgItem::Slider,        1.0,        0.0,          4.0,          0.1,        2,        true, true, tr("Saturation"), tr("")})
    << ptCfgItem({CStrengthShad,           ptCfgItem::Slider,        0.0,        0.0,          1.0,          0.05,       2,        true, true, tr("Strength"), tr("")})
    << ptCfgItem({CHueShad,                ptCfgItem::HueSlider,     240,        0,            360,          10,         0,        true, true, tr("Hue"), tr("")})
    << ptCfgItem({CSaturationMid,          ptCfgItem::Slider,        1.0,        0.0,          4.0,          0.1,        2,        true, true, tr("Saturation"), tr("")})
    << ptCfgItem({CStrengthMid,            ptCfgItem::Slider,        0.0,        0.0,          1.0,          0.05,       2,        true, true, tr("Strength"), tr("")})
    << ptCfgItem({CHueMid,                 ptCfgItem::HueSlider,     60,         0,            360,          10,         0,        true, true, tr("Hue"), tr("")})
    << ptCfgItem({CSaturationLights,       ptCfgItem::Slider,        1.0,        0.0,          4.0,          0.1,        2,        true, true, tr("Saturation"), tr("")})
    << ptCfgItem({CStrengthLights,         ptCfgItem::Slider,        0.0,        0.0,          1.0,          0.05,       2,        true, true, tr("Strength"), tr("")})
    << ptCfgItem({CHueLights,              ptCfgItem::HueSlider,     60,         0,            360,          10,         0,        true, true, tr("Hue"), tr("")})
  ;
}

//==============================================================================

bool ptFilter_Tone::doCheckHasActiveCfg() {
  return (FConfig->getValue(CStrengthAll).toFloat()    != 0.0f) || (FConfig->getValue(CSaturationAll).toFloat()    != 1.0f) ||
         (FConfig->getValue(CStrengthShad).toFloat()   != 0.0f) || (FConfig->getValue(CSaturationShad).toFloat()   != 1.0f) ||
         (FConfig->getValue(CStrengthMid).toFloat()    != 0.0f) || (FConfig->getValue(CSaturationMid).toFloat()    != 1.0f) ||
         (FConfig->getValue(CStrengthLights).toFloat() != 0.0f) || (FConfig->getValue(CSaturationLights).toFloat() != 1.0f);
}

//==============================================================================

void ptFilter_Tone::doRunFilter(ptImage *AImage) const {
  AImage->toLab();

  if ((FConfig->getValue(CStrengthAll).toFloat() != 0.0f) || (FConfig->getValue(CSaturationAll).toFloat() != 1.0f)) {
    AImage->LABTone(FConfig->getValue(CStrengthAll).toFloat(),
                    FConfig->getValue(CHueAll).toFloat(),
                    FConfig->getValue(CSaturationAll).toFloat(),
                    ptMaskType_All);
  }

  if ((FConfig->getValue(CStrengthShad).toFloat() != 0.0f) || (FConfig->getValue(CSaturationShad).toFloat() != 1.0f)) {
    AImage->LABTone(FConfig->getValue(CStrengthShad).toFloat(),
                    FConfig->getValue(CHueShad).toFloat(),
                    FConfig->getValue(CSaturationShad).toFloat(),
                    ptMaskType_Shadows);
  }

  if ((FConfig->getValue(CStrengthMid).toFloat() != 0.0f) || (FConfig->getValue(CSaturationMid).toFloat() != 1.0f)) {
    AImage->LABTone(FConfig->getValue(CStrengthMid).toFloat(),
                    FConfig->getValue(CHueMid).toFloat(),
                    FConfig->getValue(CSaturationMid).toFloat(),
                    ptMaskType_Midtones);
  }

  if ((FConfig->getValue(CStrengthLights).toFloat() != 0.0f) || (FConfig->getValue(CSaturationLights).toFloat() != 1.0f)) {
    AImage->LABTone(FConfig->getValue(CStrengthLights).toFloat(),
                    FConfig->getValue(CHueLights).toFloat(),
                    FConfig->getValue(CSaturationLights).toFloat(),
                    ptMaskType_Highlights);
  }
}

//==============================================================================

QWidget *ptFilter_Tone::doCreateGui() {
  auto hGuiBody = new QWidget;
  Ui_ToneForm hForm;

  hForm.setupUi(hGuiBody);
  this->initDesignerGui(hGuiBody);

  return hGuiBody;
}

//==============================================================================

RegisterHelper ToneRegister(&ptFilter_Tone::createTone, CToneId);

//==============================================================================