# KAccounts Integration

Integration library and QML module for Accounts-SSO and SignOn-SSO

# Introduction

KAccounts Integration provides a way to share accounts data such as login tokens and general
user information (like usernames and such) between various applications.

The KAccounts library is a KDE Frameworks style abstraction layer on top of the Accounts-SSO
and SignOnD libraries, which uses a combination of models and jobs to expose the functionality
of those.

The kaccounts QML plugin exposes that functionality directly to Qt Quick based applications,
and using the classes only requires importing the module like so:

```
import org.kde.kaccounts 1.2 as KAccounts
```

The main functionality in the library can be accessed through the various classes below, and
the accounts manager can be accessed directly through ```KAccounts::accountsManager()```. The
other central classes are:

## Models

* AccountsModel
* ServicesModel
* ProvidersModel

## Jobs

* AccountServiceToggleJob
* ChangeAccountDisplayNameJob
* CreateAccountJob
* RemoveAccountJob

# Provider and Service files

If you plan on creating new providers and services, you will need to register those with
the accounts manager. Two cmake macros are provided to assist you in the creation and
installation of these files, and further assists in translation integration for them:

* kaccounts_add_provider
* kaccounts_add_service
