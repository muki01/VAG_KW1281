# Security Policy

This project talks directly to vehicle control units and can clear fault codes, run basic settings and read memory. Security reports are therefore taken seriously.

## Supported Versions

Security fixes are applied to the latest code on the default branch and to the most recent release.

## Reporting a Vulnerability

If you find a security issue, **please do not open a public issue**.

Instead, email **muksin.muksin04@gmail.com** with:

- A description of the issue and its potential impact
- Steps to reproduce (board, configuration, sketch)
- A suggested fix, if you have one

You will receive a response as soon as possible, and credit in the release notes if you wish.

## Notes for Users

- Basic settings, actuator tests and memory access change the state of the vehicle. Use them with the vehicle stationary.
- Logs can contain identifying data such as the VIN. Remove it before you share a log.
