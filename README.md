# STM32 DC Motor Control with TB6612FNG and Encoder Feedback

Bare-metal DC motor control project built on the **STM32G070**, using the **TB6612FNG dual H-bridge motor driver** and a quadrature encoder for real-time motion feedback.

The project was developed to explore how STM32 timers, PWM, GPIO, motor drivers, DC motors, and quadrature encoders fit together as a complete embedded system. All STM32 peripheral drivers are written using **CMSIS register-level programming without HAL**.

## Demo

### Motor Control

The application automatically cycles through several motor operating states:

- Low-speed clockwise rotation
- Full-speed clockwise rotation
- Coast
- Low-speed counterclockwise rotation
- Full-speed counterclockwise rotation
- Brake

![Motor control demo](media/motor_demo.gif)

### Encoder Feedback

While the motor is being controlled, its quadrature encoder is decoded using the STM32 timer encoder interface. The application periodically calculates and reports position, direction, velocity, and distance measurements over UART.

![Encoder UART demo](media/encoder_demo.gif)

## System Architecture

The project is organized into layers so that hardware-specific behavior remains separate from higher-level application logic.

```text
Application
    │
    ├── DC Motor
    │      │
    │      └── TB6612FNG
    │              │
    │              ├── GPIO
    │              └── Timer PWM
    │
    └── Rotary Encoder
           │
           └── Timer Encoder
```

### Peripheral Drivers

Low-level STM32 peripheral interfaces provide the hardware mechanisms used by the rest of the system.

- **GPIO** — digital output and alternate-function configuration
- **Timer** — general timer configuration
- **Timer PWM** — PWM generation and direct compare-value control
- **Timer Encoder** — hardware quadrature encoder decoding
- **UART** — serial debugging and telemetry
- **SysTick** — millisecond timing

### Device Drivers

Higher-level drivers build on the peripheral layer.

#### TB6612FNG

The `tb6612fng` driver controls the dual H-bridge through GPIO and PWM and exposes operations for:

- Clockwise/counterclockwise drive
- PWM speed command
- Coast
- Short brake
- Standby control

#### DC Motor

The `dc_motor` abstraction provides motor-level operations while delegating the electrical control to the TB6612FNG driver.

The current motor speed command represents the **PWM compare value**, not a guaranteed physical velocity. A DC motor operated this way is an open-loop system: load, supply voltage, friction, and other physical effects can change the resulting RPM.

#### Rotary Encoder

The `rotary_encoder` driver builds on the STM32 timer encoder interface to provide measurements including:

- Signed encoder count
- Revolutions
- Angular position
- Direction and motion state
- RPM and revolutions per second
- Angular velocity
- Linear velocity
- Displacement and total distance

## Automated Demo

The demo application is implemented as a non-blocking state machine. Each state commands a different motor behavior while encoder measurements continue to update independently.

```text
LOW SPEED CW
     │
     ▼
FULL SPEED CW
     │
     ▼
   COAST
     │
     ▼
LOW SPEED CCW
     │
     ▼
FULL SPEED CCW
     │
     ▼
   BRAKE
     │
     ▼
 COMPLETE
```

This makes it possible to observe the difference between the **command sent to the motor** and the **motion measured by the encoder**.

In particular, the encoder demonstrates the physical difference between coasting and braking: during a coast, the shaft continues rotating as its speed gradually decreases, while short braking causes the motor to stop much more rapidly.

## Hardware

- STM32G070 development board
- TB6612FNG dual H-bridge motor driver
- Brushed DC gearmotor
- Quadrature hall encoder
- External motor power supply
- UART serial connection for telemetry

## Key Concepts Explored

This project brings together several embedded systems concepts that were previously developed independently:

- Bare-metal STM32 peripheral programming
- PWM generation with hardware timers
- Timer compare registers (`CCR`)
- H-bridge motor control
- Motor direction control
- Coast vs. short-brake behavior
- Quadrature encoder decoding
- Signed position tracking
- Velocity estimation from encoder counts
- Non-blocking application timing
- State-machine-based application design
- Layered embedded software architecture

## Open-Loop Control and Future Closed-Loop Control

The current system intentionally separates **motor actuation** from **motor measurement**.

```text
PWM Command ──► Motor Driver ──► DC Motor

                                  │
                                  ▼
                               Encoder
                                  │
                                  ▼
                           Measured Velocity
```

The PWM compare value determines how the motor is driven, but it does not guarantee a particular RPM. The encoder independently measures the resulting motion.

This provides the foundation for closed-loop motor control. A future motor-controller layer can combine the motor and encoder:

```text
Target RPM
    │
    ▼
Motor Controller / PID
    │
    ▼
PWM Command
    │
    ▼
DC Motor
    │
    ▼
Encoder Feedback
    │
    └────────────► Controller
```

That layer would allow the application to request physical quantities such as RPM or angular velocity rather than directly commanding PWM.

## Project Status

Currently implemented:

- [x] General-purpose timer driver
- [x] PWM timer driver
- [x] Timer encoder-mode driver
- [x] Quadrature rotary encoder driver
- [x] TB6612FNG driver
- [x] DC motor abstraction
- [x] PWM compare-value motor control
- [x] Encoder position and velocity measurements
- [x] Coast and short-brake control
- [x] Automated motor demonstration state machine
- [x] UART telemetry
- [ ] Generic motor-driver interface
- [ ] Closed-loop velocity control
- [ ] PID motor controller
- [ ] Dual-motor synchronization

## Motivation

This project is part of a larger effort to build up the firmware architecture required for an autonomous Micromouse robot.

Rather than treating PWM, timers, encoders, and GPIO as isolated peripherals, this project uses them together to control and observe a real electromechanical system. The eventual goal is to build on these abstractions with closed-loop motor control, allowing two drive motors to maintain precise linear and angular motion.