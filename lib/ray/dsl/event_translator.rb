module Ray
  module DSL
    # Module charged to translate an instance of Ray::Event into the arguments
    # you pass to raise_event. It is for instance used in Ray::Scene.
    #
    # Unless otherwise mentionned, points and sizes are Ray::Vector2 and
    # rects Ray::Rect.
    #
    # == Raised events
    # - quit
    # - focus_gain
    # - focus_loss
    # - mouse_entered
    # - mouse_left
    # - mouse_motion(pos)
    # - wheel_motion(pos, delta)
    # - mouse_press(button, pos)
    # - mouse_release(button, pos)
    # - key_press(key, mod_keys, native_key)
    # - key_release(key, mod_keys, native_key)
    # - window_resize(size)
    module EventTranslator
      MOUSE_BUTTONS = {
        Ray::Event::ButtonLeft    => :left,
        Ray::Event::ButtonMiddle  => :middle,
        Ray::Event::ButtonRight   => :right,
        Ray::Event::ButtonUnknown => :unknown
      }

      module_function

      # @return [Array] The arguments you should pass to raise_event.
      def translate_event(ev)
        case ev.type
        when Ray::Event::TypeQuit          then quit(ev)
        when Ray::Event::TypeFocusGain     then focus_gain(ev)
        when Ray::Event::TypeFocusLoss     then focus_loss(ev)
        when Ray::Event::TypeMouseEntered  then mouse_entered(ev)
        when Ray::Event::TypeMouseLeft     then mouse_left(ev)
        when Ray::Event::TypeMouseMotion   then mouse_motion(ev)
        when Ray::Event::TypeWheelMotion   then wheel_motion(ev)
        when Ray::Event::TypeButtonPress   then mouse_press(ev)
        when Ray::Event::TypeButtonRelease then mouse_release(ev)
        when Ray::Event::TypeKeyRelease    then key_release(ev)
        when Ray::Event::TypeKeyPress      then key_press(ev)
        when Ray::Event::TypeResize        then resize(ev)
        when Ray::Event::TypeTextEntered   then text_entered(ev)
        end
      end

      def quit(ev)
        [:quit]
      end

      def focus_gain(ev)
        [:focus_gain]
      end

      def focus_loss(ev)
        [:focus_loss]
      end

      def mouse_entered(ev)
        [:mouse_entered]
      end

      def mouse_left(ev)
        [:mouse_left]
      end

      def mouse_motion(ev)
        [:mouse_motion, ev.mouse_pos]
      end

      def wheel_motion(ev)
        [:wheel_motion, ev.mouse_pos, ev.wheel_delta]
      end

      def mouse_press(ev)
        [:mouse_press,
         MOUSE_BUTTONS[ev.button],
         ev.mouse_pos]
      end

      def mouse_release(ev)
        [:mouse_release,
         MOUSE_BUTTONS[ev.button],
         ev.mouse_pos]
      end

      def key_press(ev)
        [:key_press, ev.key, ev.key_mod, ev.native_key]
      end

      def key_release(ev)
        [:key_release, ev.key, ev.key_mod, ev.native_key]
      end

      def resize(ev)
        [:window_resize, ev.window_size]
      end

      def text_entered(ev)
        [:text_entered, ev.text]
      end
    end
  end
end
