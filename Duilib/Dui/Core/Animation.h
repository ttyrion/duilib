#pragma once
#include <memory>

namespace DuiLib {
    namespace anim {

        const std::uint16_t NONE_ANIM =          0x0000;
        // "fade"
        const std::uint16_t FADE_SHOW_ANIM =     0x0001;
        const std::uint16_t FADE_HIDE_ANIM =     0x0010;

        // "move"
        const std::uint16_t MOVE_ANIM =          0x0100;

        //

        enum ANIM_SLOT {
            FADE_SLOT = 0,
            MOVE_SLOT = 1
        };

        union anim_data {
            UINT alpha = 0;  // for fade, 
            UINT pos;          // for move, 16 bits for x and 16 bits for y
        };

        void CALLBACK AnimationTimerProc(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime);

        //class DUILIB_API StoryBoard {
        //friend class IAnimator;
        //public:
        //    virtual StoryBoard& SetAnimateUi(CControlUI* ui_element) = 0;

        //protected:
        //    virtual void OnAnimation(ANIM_TYPE type, const anim_data& data) = 0;
        //};


        class DUILIB_API IAnimator
        {
        public:
            IAnimator() {}
            virtual ~IAnimator() {}
            virtual void Start(int arg) = 0;
            virtual void Stop() = 0;

        protected:
            virtual void DoAnimation() = 0;
            friend void HandleAnimTimer(IAnimator& animator) {
                animator.DoAnimation();
            }
        };

        class DUILIB_API StoryBoard {
            friend class FadeAnimator;
        protected:
            enum ANIM_STAGE {
                STAGE_NONE,
                STAGE_PROGRESS,
                STAGE_FADE_SHOW_BEGIN,
                STAGE_FADE_SHOW_END,
                STAGE_FADE_HIDE_BEGIN,
                STAGE_FADE_HIDE_END
            };
        public:
            StoryBoard& SetAnimateUi(CControlUI* ui_element);
            StoryBoard& SetAnimator(ANIM_SLOT anim_slot, std::shared_ptr<IAnimator> animator);
            void Start(std::uint16_t anim_type);
            void Stop(std::uint16_t anim_type);
        protected:
            void OnAnimation(const std::uint16_t anim_type, const anim_data& data);
            void OnAnimationStage(const std::uint16_t anim_type, ANIM_STAGE stage);

        private:
            CControlUI* ui_element_ = nullptr;
            std::vector<std::weak_ptr<IAnimator>> animators_ = { std::weak_ptr<IAnimator>(), std::weak_ptr<IAnimator>() };
        };


        class DUILIB_API FadeAnimator : public IAnimator {
            friend class StoryBoard;
        protected:
            enum FADE_TYPE {
                None,
                SHOW, //½¥ÏÖ
                HIDE  //½¥Òþ
            };            

            const static int fade_rate = 4; //render ui per 4/1000 second

        public:
            /*
                @param ui_element : the ui control which needs a animation
                @param period     : the time of duration, in millisecond
            */
            FadeAnimator(std::shared_ptr<StoryBoard> observer, std::uint16_t period = 500, std::uint8_t min_alpha = 0, std::uint8_t max_alpha = 255);
            ~FadeAnimator();

            FadeAnimator& SetPeriod(std::uint16_t period);
            FadeAnimator& SetMaxAlpha(std::uint8_t alpha);
            FadeAnimator& SetMinAlpha(std::uint8_t alpha);
            void Start(int fade_type) override;
            void Stop() override;

        protected:
            void DoAnimation() override;

        private:
            void DoShowAnimation();
            void DoHideAnimation();

        private:
            FADE_TYPE fade_type_ = None;
            std::weak_ptr<StoryBoard> anim_observer_;
            UINT period_ = 500;
            std::uint8_t min_alpha_ = 0;
            std::uint8_t max_alpha_ = 255;
            std::uint8_t diff_alpha_ = 255;
            std::int16_t current_alpha_ = 255;
        };
    }
}