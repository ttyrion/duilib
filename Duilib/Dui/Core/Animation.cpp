#include "stdafx.h"
#include "Animation.h"
#include <atlwin.h>

namespace DuiLib {
    namespace anim {
        class CAnimWindow : public CWindowWnd {
        public:
            CAnimWindow() {
                Create(NULL, L"DuiImageUI", WS_POPUP, 0);
            }

            LPCTSTR GetWindowClassName() const override {
                return L"ANIM";
            }
        };

        HWND GetAnimWindow() {
            static CAnimWindow window;
            return window.GetHWND();
        }

        StoryBoard& StoryBoard::SetAnimateUi(CControlUI* ui_element) {
            ui_element_ = ui_element;
            return *this;
        }

        StoryBoard& StoryBoard::SetAnimator(ANIM_SLOT anim_slot, std::shared_ptr<IAnimator> animator) {
            animators_[anim_slot] = animator;

            return *this;
        }

        void StoryBoard::Start(std::uint16_t anim_type) {
            if (!ui_element_) {
                return;
            }

            if ((anim_type & FADE_SHOW_ANIM) && !animators_[FADE_SLOT].expired()) {
                animators_[FADE_SLOT].lock()->Stop();
                animators_[FADE_SLOT].lock()->Start(FadeAnimator::SHOW);
            }
            
            if ((anim_type & FADE_HIDE_ANIM) && !animators_[FADE_SLOT].expired()) {
                animators_[FADE_SLOT].lock()->Stop();
                animators_[FADE_SLOT].lock()->Start(FadeAnimator::HIDE);
            }
            
            if ((anim_type & MOVE_ANIM) && !animators_[MOVE_SLOT].expired()) {
                animators_[MOVE_SLOT].lock()->Stop();
                animators_[MOVE_SLOT].lock()->Start(0);
            }
        }

        void StoryBoard::Stop(std::uint16_t anim_type) {
            if ((anim_type & FADE_SHOW_ANIM) && !animators_[FADE_SLOT].expired()) {
                animators_[FADE_SLOT].lock()->Stop();
            }
            else if ((anim_type & FADE_HIDE_ANIM) && !animators_[FADE_SLOT].expired()) {
                animators_[FADE_SLOT].lock()->Stop();
            }

            if ((anim_type & MOVE_ANIM) && !animators_[MOVE_SLOT].expired()) {
                animators_[MOVE_SLOT].lock()->Stop();
            }
        }

        void StoryBoard::OnAnimation(const std::uint16_t anim_type, const anim_data& data) {
            if (!ui_element_) {
                return;                
            }

            switch (anim_type) {
            case FADE_SHOW_ANIM:
            case FADE_HIDE_ANIM: {
                ui_element_->SetAlpha(data.alpha);
                ui_element_->Invalidate();
                break;
            }
            case MOVE_ANIM: {
                RECT pos;
                pos.left = GETXPOS(data.pos);
                pos.top = GETYPOS(data.pos);
                pos.right = pos.left + ui_element_->GetWidth();
                pos.bottom = pos.top + ui_element_->GetHeight();
                ui_element_->SetPos(pos);

                break;
            }
            default:
                break;
            }
        }

        void StoryBoard::OnAnimationStage(const std::uint16_t anim_type, ANIM_STAGE stage) {
            if (!ui_element_) {
                return;
            }

            switch (anim_type) {
            case FADE_SHOW_ANIM: {
                if (stage == STAGE_FADE_SHOW_BEGIN) {
                    if (!ui_element_->IsVisible()) {
                        ui_element_->SetVisible(true);
                    }
                }
                //else if (stage == STAGE_FADE_SHOW_END) {

                //}
            }
            case FADE_HIDE_ANIM:
                if (stage == STAGE_FADE_HIDE_END) {
                    if (ui_element_->IsVisible()) {
                        ui_element_->SetVisible(false);
                    }
                }

                break;
            default:
                break;
            }
        }

        FadeAnimator::FadeAnimator(std::shared_ptr<StoryBoard> observer, std::uint8_t min_alpha, std::uint8_t max_alpha, std::uint16_t period) {
            anim_observer_ = observer;
            period_ = period;
            max_alpha_ = max_alpha;
            min_alpha_ = min_alpha;
            diff_alpha_ = max_alpha_ - min_alpha_;
        }

        FadeAnimator::~FadeAnimator() {
            
        }

        FadeAnimator& FadeAnimator::SetAnimationFactor(ANIM_FACTOR_NAME key, UINT value) {
            if (key == ANIM_FACTOR_PERIOD) {
                SetPeriod(value & 0xFFFF);
            }
            else if (key == ANIM_FACTOR_FADE_MAXA) {
                SetPeriod(value & 0xFF);
            }
            else if (key == ANIM_FACTOR_FADE_MINA) {
                SetPeriod(value & 0xFF);
            }

            return *this;
        }

        FadeAnimator& FadeAnimator::SetPeriod(std::uint16_t period) {
            period_ = period;
            return *this;
        }
        FadeAnimator& FadeAnimator::SetMaxAlpha(std::uint8_t alpha) {
            max_alpha_ = alpha;
            return *this;
        }

        FadeAnimator& FadeAnimator::SetMinAlpha(std::uint8_t alpha) {
            min_alpha_ = alpha;
            return *this;
        }

        void FadeAnimator::Start(int fade_type) {
            fade_type_ = (FADE_TYPE)fade_type;
            switch (fade_type_)
            {
            case DuiLib::anim::FadeAnimator::None:
                return;
            case DuiLib::anim::FadeAnimator::SHOW:
                current_alpha_ = min_alpha_;
                break;
            case DuiLib::anim::FadeAnimator::HIDE:
                current_alpha_ = max_alpha_;
                break;
            default:
                break;
            }

            ::SetTimer(GetAnimWindow(), (UINT_PTR)this, fade_rate, (TIMERPROC)AnimationTimerProc);
        }

        void FadeAnimator::Stop() {
            ::KillTimer(GetAnimWindow(), (UINT_PTR)this);
        }

        void FadeAnimator::DoAnimation() {
            if (fade_type_ == SHOW) {
                DoShowAnimation();
            }
            else {
                DoHideAnimation();
            }
        }

        void FadeAnimator::DoShowAnimation() {
            if (!anim_observer_.expired()) {
                StoryBoard::ANIM_STAGE stage = StoryBoard::STAGE_NONE;
                if (current_alpha_ == min_alpha_) {
                    stage = StoryBoard::STAGE_FADE_SHOW_BEGIN;
                }

                current_alpha_ += diff_alpha_ / (period_ / fade_rate);
                if (current_alpha_ >= max_alpha_) {
                    Stop();
                    current_alpha_ = max_alpha_;
                    stage = StoryBoard::STAGE_FADE_SHOW_END;
                }

                anim_data data;
                data.alpha = current_alpha_;
                anim_observer_.lock()->OnAnimation(FADE_SHOW_ANIM, data);
                anim_observer_.lock()->OnAnimationStage(FADE_SHOW_ANIM, stage);
            }
        }

        void FadeAnimator::DoHideAnimation() {
            if (!anim_observer_.expired()) {
                StoryBoard::ANIM_STAGE stage = StoryBoard::STAGE_NONE;
                if (current_alpha_ == max_alpha_) {
                    stage = StoryBoard::STAGE_FADE_HIDE_BEGIN;
                }

                current_alpha_ -= diff_alpha_ / (period_ / fade_rate);
                if (current_alpha_ <= min_alpha_) {
                    Stop();
                    current_alpha_ = min_alpha_;
                    stage = StoryBoard::STAGE_FADE_HIDE_END;
                }

                anim_data data;
                data.alpha = current_alpha_;
                anim_observer_.lock()->OnAnimation(FADE_HIDE_ANIM, data);
                anim_observer_.lock()->OnAnimationStage(FADE_HIDE_ANIM, stage);
            }
        }

        MovingAnimator::MovingAnimator(std::shared_ptr<StoryBoard> observer, POINT start, POINT dest, std::uint16_t period) {
            anim_observer_ = observer;
            start_pt_ = start;
            dest_pt_ = dest;
            period_ = period;
        }

        MovingAnimator::~MovingAnimator() {
            
        }

        MovingAnimator& MovingAnimator::SetAnimationFactor(ANIM_FACTOR_NAME key, UINT value) {
            if (key == ANIM_FACTOR_PERIOD) {
                SetPeriod(value & 0xFFFF);
            }
            else if (key == ANIM_FACTOR_MOVE_START) {
                POINT pt;
                pt.x = GETXPOS(value);
                pt.y = GETYPOS(value);
                SetStartPos(pt);
            }
            else if (key == ANIM_FACTOR_MOVE_DEST) {
                POINT pt;
                pt.x = GETXPOS(value);
                pt.y = GETYPOS(value);
                SetDestPos(pt);
            }

            return *this;
        }

        MovingAnimator& MovingAnimator::SetPeriod(std::uint16_t period) {
            period_ = period;

            return *this;
        }

        MovingAnimator& MovingAnimator::SetStartPos(POINT start) {
            start_pt_ = start;

            return *this;
        }

        MovingAnimator& MovingAnimator::SetDestPos(POINT dest) {
            dest_pt_ = dest;

            return *this;
        }

        void MovingAnimator::Start(int type_no_use) {
            // Use a local coordinate
            dest_pt_.x -= start_pt_.x;
            dest_pt_.y -= start_pt_.y;
            current_x_ = 0;
            moving_slope_ = dest_pt_.y / (float)dest_pt_.x;

            lambda_ = [] (LONG x, float moving_slope) ->POINT {
                LONG y = moving_slope * x;
                POINT pt;
                pt.x = x;
                pt.y = y;
                return pt;
            };

            ::SetTimer(GetAnimWindow(), (UINT_PTR)this, move_rate, (TIMERPROC)AnimationTimerProc);
        }

        void MovingAnimator::Stop() {
            ::KillTimer(GetAnimWindow(), (UINT_PTR)this);
        }

        void MovingAnimator::DoAnimation() {
            current_x_ += dest_pt_.x / (period_ / move_rate);
            if (current_x_ > dest_pt_.x) {
                current_x_ = dest_pt_.x;
                Stop();
            }

            POINT current_pt = lambda_(current_x_, moving_slope_);
            current_pt.x += start_pt_.x;
            current_pt.y += start_pt_.y;

            if (!anim_observer_.expired()) {
                anim_data data;
                data.pos = MAKEPOS(current_pt.x, current_pt.y);
                anim_observer_.lock()->OnAnimation(MOVE_ANIM, data);
            }
        }

        void CALLBACK AnimationTimerProc(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime) {
            IAnimator* anim = reinterpret_cast<IAnimator*>(nTimerid);
            HandleAnimTimer(*anim);
        }
    }
}
