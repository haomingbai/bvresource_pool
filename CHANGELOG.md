# Changelog

## Unreleased

### Changed

* Migrated async completion-token integration to modern Boost.Asio initiation model.
* Updated async executor storage to `boost::asio::any_io_executor`.
* Updated strand example to `boost::asio::make_strand`.
* Added `async_regression` smoke check with compile-time header/template instantiation checks and a runtime async acquire/release/reacquire path.
* CI now covers GCC and Clang in both Debug and Release modes.
* CI validates multiple Boost lines via Ubuntu 22.04 (Boost 1.74) and Ubuntu 24.04 (newer Boost).

### Breaking changes

* Raised minimum supported Boost version to **1.74**.
