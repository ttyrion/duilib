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
                animators_[FADE_SLOT].lock()->Start(FadeAnimator::SHOW);
            }
            else if ((anim_type & FADE_HIDE_ANIM) && !animators_[FADE_SLOT].expired()) {
                animators_[FADE_SLOT].lock()->Start(FadeAnimator::HIDE);
            }

            if ((anim_type & MOVE_ANIM) && !animators_[MOVE_SLOT].expired()) {
                //
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
                //
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

        FadeAnimator::FadeAnimator(std::shared_ptr<StoryBoard> observer, std::uint16_t period, std::uint8_t min_alpha, std::uint8_t max_alpha) {
            anim_observer_ = observer;
            period_ = period;
            max_alpha_ = max_alpha;
            min_alpha_ = min_alpha;
            diff_alpha_ = max_alpha_ - min_alpha_;
        }

        FadeAnimator::~FadeAnimator() {
            
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

        void CALLBACK AnimationTimerProc(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime) {
            FadeAnimator* anim = reinterpret_cast<FadeAnimator*>(nTimerid);
            HandleAnimTimer(*anim);
        }
    }
}
