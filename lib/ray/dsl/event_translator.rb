module Ray
  module DSL
    # Module charged to translate an instance of Ray::Event into the arguments
    # you pass to raise_event.
    module EventTranslator
      FOCUS_TYPES = {
        Ray::Event::APPMOUSEFOCUS => :mouse_focus,
        Ray::Event::APPINPUTFOCUS => :input_focus,
        Ray::Event::APPACTIVE     => :app_activity
      }

      MOUSE_BUTTONS = {
        Ray::Event::BUTTON_LEFT      => :left,
        Ray::Event::BUTTON_MIDDLE    => :middle,
        Ray::Event::BUTTON_RIGHT     => :right,
        Ray::Event::BUTTON_WHEELUP   => :wheel_up,
        Ray::Event::BUTTON_WHEELDOWN => :wheel_down
      }

      class << self
        # @return [Array, nil] The arguments you should pass to raise_event.
        def translate_event(ev)
          case ev.type
          when 0 then []
          when Ray::Event::TYPE_QUIT then quit(ev)
          when Ray::Event::TYPE_ACTIVEEVENT then active_event(ev)
          when Ray::Event::TYPE_MOUSEMOTION then mouse_motion(ev)
          when Ray::Event::TYPE_MOUSEBUTTONDOWN then mouse_press(ev)
          when Ray::Event::TYPE_MOUSEBUTTONUP then mouse_release(ev)
          when Ray::Event::TYPE_KEYDOWN then key_press(ev)
          when Ray::Event::TYPE_KEYUP then key_release(ev)
          when Ray::Event::TYPE_VIDEORESIZE then resize(ev)
          when Ray::Event::TYPE_JOYAXISMOTION then joy_motion(ev)
          when Ray::Event::TYPE_JOYBUTTONDOWN then joy_press(ev)
          when Ray::Event::TYPE_JOYBUTTONUP then joy_release(ev)
          else []
          end
        end

        def quit(ev)
          [[:quit]]
        end

        def active_event(ev)
          [[ev.is_gain? ? :focus_gain : :focus_loss,
            FOCUS_TYPES[ev.focus_state]]]
        end

        def mouse_motion(ev)
          [[:mouse_motion, Ray::Rect.new(ev.mouse_x, ev.mouse_y)]]
        end

        def mouse_press(ev)
          [[:mouse_press,
            MOUSE_BUTTONS[ev.mouse_button],
            Ray::Rect.new(ev.mouse_x, ev.mouse_y)]]
        end

        def mouse_release(ev)
          [[:mouse_release,
            MOUSE_BUTTONS[ev.mouse_button],
            Ray::Rect.new(ev.mouse_x, ev.mouse_y)]]
        end

        def key_press(ev)
          [[:key_press, ev.key, ev.mod_keys]]
        end

        def key_release(ev)
          [[:key_release, ev.key, ev.mod_keys]]
        end

        def resize(ev)
          [[:window_resize, Ray::Rect.new(0, 0, ev.window_w, ev.window_h)]]
        end

        def joy_motion(ev)
          [[:joy_motion, ev.joystick_id, ev.axis_id, ev.axis_value]]
        end

        def joy_press(ev)
          [[:joy_press, ev.joystick_id, ev.joystick_button]]
        end

        def joy_release(ev)
          [[:joy_release, ev.joystick_id, ev.joystick_button]]
        end
      end
    end
  end
end
