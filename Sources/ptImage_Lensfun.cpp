/*******************************************************************************
**
** Photivo
**
** Copyright (C) 2011 Bernd Schoeler <brjohn@brother-john.net>
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
** along with Photivo.  If not, see <http:**www.gnu.org/licenses/>.
**
*******************************************************************************/

#include <lensfun.h>
#include "ptImage.h"

typedef float Tfloat;   // needed by the interpolation code

ptImage* ptImage::Lensfun(const int LfunActions, const lfModifier* LfunData) {

  // Stage 2: Vignetting.
  if ((LfunActions & LF_MODIFY_VIGNETTING) ||
      (LfunActions & LF_MODIFY_CCI) ||   // TODO: not yet supported by Photivo
      (LfunActions == LF_MODIFY_ALL) )
  {
    LfunData->ApplyColorModification(m_Image, 0.0, 0.0, m_Width, m_Height,
                                     LF_CR_3(RED, GREEN, BLUE),
                                     m_Width * 3 * sizeof(uint16_t));
  }


  // Stage 1 and/or 3: CA, lens Geometry/distortion
  // Processing row by row to avoid *huge* memory requirements
  if (((LfunActions & LF_MODIFY_TCA) || (LfunActions & LF_MODIFY_DISTORTION) || (LfunActions & LF_MODIFY_GEOMETRY)) ||
      (LfunActions == LF_MODIFY_ALL) )
  {
    float* NewCoords = (float*) CALLOC((uint32_t)m_Width * 3 * 2, sizeof(float));
    uint16_t (*TempImage)[3] = (uint16_t (*)[3]) CALLOC((int32_t)m_Width * m_Height, sizeof(*TempImage));

    for (uint16_t row = 0; row < m_Height; row++) {
      int32_t Temp = row * m_Width;
      LfunData->ApplySubpixelDistortion(0.0, row, m_Width, 1, NewCoords);

      // Bicubic interpolation
      for (uint16_t col = 0; col < m_Width; col++) {
        if (NewCoords[col*2] < 0 || NewCoords[col*2] > m_Width-1 || NewCoords[col*2+1] < 0 || NewCoords[col*2+1] > m_Height-1) {
          for (short c = 0; c < 3; c++) {
            TempImage[Temp+col][c] = 0;
          }
          continue;
        }
        const int32_t
          x = (int32_t)NewCoords[col*2],
          y = (int32_t)NewCoords[col*2+1];
        const float
          dx = NewCoords[col*2] - x,
          dy = NewCoords[col*2+1] - y;
        const int32_t
          px = x-1<0?0:x-1, nx = dx>0?x+1:x, ax = x+2>=m_Width?m_Width-1:x+2,
          py = y-1<0?0:y-1, ny = dy>0?y+1:y, ay = y+2>=m_Height?m_Height-1:y+2;
        const int32_t
          pyw = py * m_Width, yw = y * m_Width,
          nyw = ny * m_Width, ayw = ay * m_Width;
        for (short c = 0; c < 3; c++) {
          const Tfloat
            Ipp = m_Image[px+pyw][c], Icp = m_Image[x+pyw][c], Inp = m_Image[nx+pyw][c], Iap = m_Image[ax+pyw][c],
            Ip = Icp + 0.5f*(dx*(-Ipp+Inp) + dx*dx*(2*Ipp-5*Icp+4*Inp-Iap) + dx*dx*dx*(-Ipp+3*Icp-3*Inp+Iap)),
            Ipc = m_Image[px+yw][c], Icc = m_Image[x+yw][c], Inc = m_Image[nx+yw][c], Iac = m_Image[ax+yw][c],
            Ic = Icc + 0.5f*(dx*(-Ipc+Inc) + dx*dx*(2*Ipc-5*Icc+4*Inc-Iac) + dx*dx*dx*(-Ipc+3*Icc-3*Inc+Iac)),
            Ipn = m_Image[px+nyw][c], Icn = m_Image[x+nyw][c], Inn = m_Image[nx+nyw][c], Ian = m_Image[ax+nyw][c],
            In = Icn + 0.5f*(dx*(-Ipn+Inn) + dx*dx*(2*Ipn-5*Icn+4*Inn-Ian) + dx*dx*dx*(-Ipn+3*Icn-3*Inn+Ian)),
            Ipa = m_Image[px+ayw][c], Ica = m_Image[x+ayw][c], Ina = m_Image[nx+ayw][c], Iaa = m_Image[ax+ayw][c],
            Ia = Ica + 0.5f*(dx*(-Ipa+Ina) + dx*dx*(2*Ipa-5*Ica+4*Ina-Iaa) + dx*dx*dx*(-Ipa+3*Ica-3*Ina+Iaa));
          const Tfloat val = Ic + 0.5f*(dy*(-Ip+In) + dy*dy*(2*Ip-5*Ic+4*In-Ia) + dy*dy*dy*(-Ip+3*Ic-3*In+Ia));
          TempImage[Temp+col][c] = CLIP((int32_t) val);
        }
      }
    }

    FREE(NewCoords);
    FREE(m_Image);
    m_Image = TempImage;
  }

  return this;
}
