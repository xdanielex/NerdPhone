# Difficulty negotiation research

Research date: 2026-11-11. Short observations are not a guarantee about future server settings.

## Original NerdMiner flow

The referenced firmware's `src/stratum.cpp` sends:

- `mining.subscribe` with `NerdMinerV2/` plus `CURRENT_VERSION`.
- `mining.authorize` with payout address and pool password.
- `mining.suggest_difficulty` to request a target.

`src/version.h` defines `CURRENT_VERSION` as `V1.8.3` in the pinned revision, giving **`NerdMinerV2/V1.8.3`** (including the second V). `src/mining.h` defines the original default suggested difficulty as **0.00015**, not 0.001. The Android application deliberately requests **0.001** as requested by its developer. It already used the original user agent before this research.

Sources:
- https://github.com/BitMaker-hub/NerdMiner_v2/blob/e3a04b7d9f0b6e60ca6e02f7433c94d78d9dded4/src/stratum.cpp
- https://github.com/BitMaker-hub/NerdMiner_v2/blob/e3a04b7d9f0b6e60ca6e02f7433c94d78d9dded4/src/version.h
- https://github.com/BitMaker-hub/NerdMiner_v2/blob/e3a04b7d9f0b6e60ca6e02f7433c94d78d9dded4/src/mining.h

## NerdMiner.io documentation

The operator's site documents `pool.nerdminer.io:3333`, a valid BTC payout address and password `x`. It advertises automatic difficulty adjustment, but the page reviewed does not establish that requesting 0.001 always overrides server policy. It also requires an actual valid mainnet address; it does not document a magic worker suffix to force 0.001.

Source: https://nerdminer.io/

## Short live probes

Two short authorized TCP sessions used the original `NerdMinerV2/V1.8.3` identifier and a fresh, generated valid test-only address. No real user wallet was used, no shares were submitted, and no private keys were printed or saved. Each session observed the server for roughly seven seconds.

1. Password `x` + `mining.suggest_difficulty [0.001]`: subscribe succeeded, authorize returned true, jobs arrived, and the server announced difficulty **1**. No successful assignment of 0.001 was observed.
2. Experimental password hint `d=0.001` + the same RPC request: subscribe/authorize succeeded, but the observed assignments were again **1**.

Transcripts: [normal password](nerdminer-pool-x-probe.json), [experimental hint](nerdminer-pool-diffhint-probe.json). Extranonces and job IDs are ephemeral protocol values, not wallet secrets. The hint test does not establish that the backend implements that syntax; it is not enabled by default.

Earlier unauthed, rapid user-agent probes were inconclusive (some sessions were immediately closed). They are not evidence of user-agent-based difficulty selection and were not used to change defaults.

**Conclusion:** changing to the exact original user agent cannot be presented as a new fix, because the app already sends it. The short tests did not obtain 0.001. They do not prove a permanent minimum, identify the backend implementation, or rule out later vardiff changes. Longer operation and/or an operator response would be needed for those claims.

## Public Pool: relevant but a different server

The separately maintained open-source Public Pool code handles `mining.suggest_difficulty`, stores it as a session difficulty, and emits `mining.set_difficulty`. Its statistics code includes a minimum of 0.00001 at the revision inspected. That does **not** demonstrate the behavior/configuration of NerdMiner.io or even of a managed Public Pool deployment.

Inspected revision: `96a9202c11de2c6fc8d41155e2e779912a476dc7`.

- https://github.com/benjamin-wilson/public-pool/blob/96a9202c11de2c6fc8d41155e2e779912a476dc7/src/models/StratumV1Client.ts
- https://github.com/benjamin-wilson/public-pool/blob/96a9202c11de2c6fc8d41155e2e779912a476dc7/src/models/StratumV1ClientStatistics.ts

A June 2026 issue reports a managed Public Pool endpoint keeping difficulty 1 even for real NerdMiner hardware and low suggestions. It is a user report about a different service, not confirmation of NerdMiner.io policy:

https://github.com/benjamin-wilson/public-pool/issues/152

## What the app does

- Preserves the documented NerdMiner-compatible subscribe identifier, visible in the UI.
- Requests 0.001 at handshake, with the existing low-frequency keepalive requests.
- Counts requests and displays any RPC response/error received for their IDs.
- Shows the latest pool-assigned difficulty separately from the current job's target.
- Never forces the local target to 0.001 while submitting those shares as difficulty 1.
- Does not implement undocumented identity/password guessing as an automatic strategy.

For an operator support request, report host/port, the exact UA, the requested value, assigned value and a sanitized transcript, and ask whether the pool permits a static 0.001 target or offers a dedicated low-difficulty endpoint.
