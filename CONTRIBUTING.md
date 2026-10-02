# Contributing to VAG KW1281

Thank you for taking the time to contribute! Every vehicle test, recorded response, fix and idea makes this project better for everyone who works on Volkswagen Group cars.

By participating, you agree to follow the [Code of Conduct](CODE_OF_CONDUCT.md).

## Ways to Contribute

| | |
|---|---|
| 🚗 **Report a tested vehicle** | Tell us which cars and control units work (or don't). Use the **Vehicle report** issue template. |
| 📼 **Share recorded responses** | Blocks captured from other control units make the simulator and the value table more useful. |
| 🐛 **Report a bug** | Use the **Bug report** template and include the serial log whenever possible. |
| 💡 **Suggest a feature** | Use the **Feature request** template. |
| 🔧 **Submit code** | Protocol fixes, new block types, corrections to the value formulas. |
| 📝 **Improve the docs** | Clearer explanations, wiring photos, ECU pinouts. |

## Reporting Bugs

Before opening an issue, please search the [existing issues](https://github.com/muki01/VAG_KW1281/issues). A good report includes:

- The sketch (`VAG_KW1281`, `Basic_Communication_Test` or `Basic_KW1281_Simulator`) and the commit you are using
- The board and the Arduino core version
- The interface circuit (transistor, LM393, L9637D, MC33290, …)
- The vehicle and the control unit: make, model, year, module and ECU part number
- **The serial debug output.** The blocks sent and received, with their counters, are the most useful information.

## Development Workflow

1. **Fork** the repository and create a branch:
   ```bash
   git checkout -b feature/my-improvement
   ```
2. Make your changes, keeping them **focused**: one fix or feature per pull request.
3. **Test on a real control unit** when your change touches the wake-up, the block handling or the timing, and say in the pull request which vehicle and module you tested on.
4. Make sure everything still **compiles** for the boards it supports.
5. Commit with a clear message.
6. Push and open a **pull request**, filling in the template.

## Coding Guidelines

- Follow the existing style of the file you are editing: naming, indentation and comment density.
- Keep recorded responses in `Codes.h`, with a comment that says which vehicle they come from.
- Do not change the byte and block timings without testing on a control unit.
- Do not commit personal data (for example a VIN or an immobiliser code) in code or logs.

## License

By contributing, you agree that your contributions are licensed under the [GNU General Public License v3.0](LICENSE), and that the author may also offer them under a commercial license.
